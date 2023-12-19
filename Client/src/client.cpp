#include "../include/client.h"

#pragma warning(disable: 4996)

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

namespace TCPChat
{
	Client::Client() noexcept
		:
		wsa{ 0 },
		client_socket(INVALID_SOCKET),
		client_info{ 0 },
		client_info_lenght(sizeof(client_info)),
		server_info{ 0 },
		server_info_lenght(sizeof(server_info)),
		recieved_message_size(0) {}


	bool Client::Connect(std::string ip, int port)
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("WSAStartup");
			return false;
		}

		if ((client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("socket");
			return false;
		}

		char mode = '1';

		if ((setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &mode, sizeof(int)) == SOCKET_ERROR))
		{
			std::cout << WSAGetLastError() << '\n';
			perror("setsockport");
			return false;
		}

		server_info.sin_family = PF_INET;
		server_info.sin_port = htons(port);
		server_info.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

		ZeroMemory(server_info.sin_zero, 8);

		if (connect(client_socket, (const sockaddr*)&server_info, server_info_lenght) == INVALID_SOCKET)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("client connect");
			return false;
		}

		if (getsockname(client_socket, (sockaddr*)&client_info, &client_info_lenght) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("getsockname");
			return false;
		}


		GetClientExternalIp();
		//SendUserInfo();

		std::cout << std::format("Client info\nExternal ip: {} port: {}\n", client_external_ip, client_info.sin_port);
		

		is_connected = true;

		return is_connected;
	}

	void Client::GetClientExternalIp()
	{
		HINTERNET net = InternetOpenW(L"IP retriever", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		HINTERNET conn = InternetOpenUrlW(net, L"http://myexternalip.com/raw", NULL, 0, INTERNET_FLAG_RELOAD, 0);

		char buffer[4096];
		DWORD read;

		InternetReadFile(conn, buffer, sizeof(buffer) / sizeof(buffer[0]), &read);
		InternetCloseHandle(net);

		client_external_ip = std::string(buffer, read);
	}

	void Client::SendUserInfo()
	{
		std::cout << "sign up(1) or sign in(2)\n";
		int i = 0;
		bool is_good = false;

		while (!is_good)
		{
			std::cin >> i;
			uinfo.type = static_cast<ConnectionType>(i);

			switch (uinfo.type)
			{
			case ConnectionType::SIGN_UP:
			{
				std::cout << "SIGN UP\n";

				std::cout << "username: ";
				std::cin >> uinfo.username;

				std::cout << "login: ";
				std::cin >> uinfo.login;

				std::cout << "password: ";
				std::cin >> uinfo.password;
				std::cout << '\n';

				std::cout << "CLIENT SOCKET: " << client_socket << '\n';

				char* send_buffer = new char[sizeof(Client::user_info)];
				memcpy(send_buffer, &uinfo, sizeof(Client::user_info));

				if (send(client_socket, send_buffer, sizeof(Client::user_info), 0) <= 0)
				{
					std::cout << WSAGetLastError() << '\n';
					perror("send uinfo");
					delete[] send_buffer;
					return;
				}

				delete[] send_buffer;

				std::cout << "CLIENT SOCKET: " << client_socket << '\n';

				is_good = true;
			} break;

			case ConnectionType::SIGN_IN:
			{
				std::cout << "SIGN IN\n";

				std::cout << "login: ";
				std::cin >> uinfo.login;

				std::cout << "password: ";
				std::cin >> uinfo.password;
				std::cout << '\n';

				std::cout << "CLIENT SOCKET: " << client_socket << '\n';
				char* send_buffer = new char[sizeof(Client::user_info)];
				memcpy(send_buffer, &uinfo, sizeof(Client::user_info));
				if (send(client_socket, send_buffer, sizeof(Client::user_info), 0) <= 0)
				{
					std::cout << WSAGetLastError() << '\n';
					perror("send uinfo");
					delete[] send_buffer;
					return;
				}

				delete[] send_buffer;

				RecieveUsersInfo();

				is_good = true;
			} break;

			default:
			{
				i = 0;
				std::cout << "enter 1(sign up) or 2(sign in): ";
			} break;
			}
			//is_good = true;
		}

		std::cout << "connection type: " << uinfo.type << '\n';
		std::cout << std::format("username: {}\n", uinfo.username);
		std::cout << std::format("login: {}\n", uinfo.login);
		std::cout << std::format("password: {}\n", uinfo.password);
		std::cout << std::format("sizeof: {}\n", sizeof(Client::user_info));
	}

	void Client::RecieveUsersInfo()
	{
		std::vector<char> recieved_buffer;
		int recieved_buffer_size = 0;

		if (recv(client_socket, (char*)(&uinfo_dto.client_count), sizeof(int), 0) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recv uinfo_dto size");
			return;
		}
		
		if (recv(client_socket, (char*)(&recieved_buffer_size), sizeof(int), 0) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recv uinfo_dto");
			return;
		}

		std::cout << uinfo_dto.client_count << '\n';

		recieved_buffer.resize(recieved_buffer_size, 0x00);

		if (recv(client_socket, recieved_buffer.data(), recieved_buffer_size, 0) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recv uinfo_dto");
			return;
		}

		for (int i = 0; i != recieved_buffer.size(); ++i)
		{
			std::cout << recieved_buffer[i];
		}
		std::cout << '\n';

		std::string tmp;
		for (const auto& byte : recieved_buffer)
		{
			if (byte == '\0')
			{
				uinfo_dto.usernames.push_back(tmp);
				tmp.clear();
			}
			else
			{
				tmp += byte;
			}
		}
		
		std::cout << "USERNAMES: ";
		for (const auto& username : uinfo_dto.usernames)
		{
			std::cout << username << '\n';
		}
	}

#if 0
	void Client::SendData()
	{
		std::cout << "Send message: ";
		std::getline(std::cin, send_message);

		send_message_size = send_message.size();

		if (sendto(client_socket, (char*)&send_message_size, sizeof(int), 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto message size");
			return;
		}

		if (sendto(client_socket, send_message.c_str(), send_message_size, 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto message");
			return;
		}

		int client = static_cast<int>(client_type);

		if (sendto(client_socket, (char*)&client, sizeof(int), 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto message");
			return;
		}
	}

	void Client::RecieveData()
	{
		while (true)
		{
			std::unique_lock<std::mutex> recv_lock(recieve_mutex);

			cv.wait(recv_lock, [this] {
				return is_connected;
				});

			if (recvfrom(client_socket, (char*)&recieve_message_size, sizeof(int), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom message size");
				return;
			}

			recieve_message = new char[recieve_message_size + 1];
			recieve_message[recieve_message_size] = '\0';

			if (recvfrom(client_socket, recieve_message, recieve_message_size, 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom message");
				return;
			}

			std::cout << recieve_message << '\n';

			delete[] recieve_message;
	}
}
#endif
	void Client::Disconnect()
	{
		std::cout << "Disconnect\n";
		shutdown(client_socket, SD_SEND);
		closesocket(client_socket);
		is_connected = false;
	}


	void Client::Run()
	{
		while (true);

		Disconnect();

		//BOOL bNewBehavior = FALSE;
		//DWORD dwBytesReturned = 0;
		//WSAIoctl(client_socket, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);

		//SendClientInfo();

		//recieve_thread = (std::thread(&Client::RecieveData, this));

		//while (is_connected)
		//{
			//SendData();
		//}
	}

	Client::~Client()
	{
		WSACleanup();
	}
}