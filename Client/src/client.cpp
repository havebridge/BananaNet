#include "client.h"


#pragma warning(disable: 4996)

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

namespace UDPChat
{
	Client::Client() noexcept
		:
		wsa{ 0 },
		client_socket(INVALID_SOCKET),
		server_info{ 0 },
		server_info_lenght(sizeof(server_info)),
		client_local_ip(NULL),
		client_info{ 0 },
		client_info_lenght(sizeof(client_info)),
		is_connected(false),
		recieve_message_size(0) {}


	bool Client::Connect(std::string ip, int port)
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("WSAStartup");
			return false;
		}

		if ((client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("socket");
			return false;
		}

		char yes = '1';

		if ((setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == SOCKET_ERROR))
		{
			std::cout << WSAGetLastError() << '\n';
			perror("setsockport");
			return false;
		}

		server_info.sin_family = AF_INET;
		server_info.sin_port = htons(port);
		server_info.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

		ZeroMemory(server_info.sin_zero, 8);

		return true;
	}

	//TODO(hasbridge): get internal ip function

	std::string Client::GetClientExternalIp() const
	{
		HINTERNET net = InternetOpenW(L"IP retriever", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		HINTERNET conn = InternetOpenUrlW(net, L"http://myexternalip.com/raw", NULL, 0, INTERNET_FLAG_RELOAD, 0);

		char buffer[4096];
		DWORD read;

		InternetReadFile(conn, buffer, sizeof(buffer) / sizeof(buffer[0]), &read);
		InternetCloseHandle(net);

		return std::string(buffer, read);
	}

	bool Client::ProcessHandlerFile(const char* file_name)
	{
		client_handler_file.open(file_name);

		if (client_handler_file.is_open())
		{
			int res;
			client_handler_file >> res;

			client_type = static_cast<Instance::type>(res);

			std::cout << res;

			client_handler_file.close();
			return true;
		}
		else
		{
			std::cerr << "Unable to open the file\n";
			return false;
		}
	}

	bool Client::SendClientInfo()
	{
		client_external_ip = GetClientExternalIp();

		if (sendto(client_socket, (char*)&is_connected, sizeof(bool), 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto first client ip");
			return false;
		}

		char send_info_to_server = '1';

		if (sendto(client_socket, &send_info_to_server, sizeof(char), 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto char");
			return false;
		}

		client_info_lenght = sizeof(client_info);

		getsockname(client_socket, (sockaddr*)&client_info, &client_info_lenght);
		client_info.sin_addr.s_addr = inet_addr(client_external_ip.c_str());

		inet_pton(AF_INET, inet_ntoa(client_info.sin_addr), &(client_info.sin_addr));

		printf("Client %s:%d is connected to UDP server %s:%d\n",
			inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port),
			inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));

		is_connected = true;

		cv.notify_one();
//client_handler_file.open("../../../handler/file_handler.txt");
		ProcessHandlerFile("../../../handler/file_handler.txt");

		return is_connected;
	}

	void Client::SendData()
	{
#if DEBUG
		std::cout << "SendMSG\n";
#endif
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
#if DEBUG
		std::cout << "message size send: " << send_message_size;
		std::cout << "\nmessage send:" << send_message.data();
		std::cout << "\nclient type send:" << client;
		std::cout << '\n';
#endif
	}

	void Client::RecieveData()
	{
		while (true)
		{
			std::unique_lock<std::mutex> recv_lock(recieve_mutex);

			cv.wait(recv_lock, [this] {
				return is_connected;
				});

#if DEBUG
			std::cout << "RecvMSG\n";
#endif
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

			std::cout << "\nmessage recv: " << recieve_message;
			std::cout << '\n';

#if DEBUG
			std::cout << "RECV MESSAGE\n";
			std::cout << "message size recv: " << recieve_message_size;
			std::cout << "\nmessage recv: " << recieve_message;
			std::cout << '\n';
#endif
			delete[] recieve_message;
		}
	}


	void Client::Run()
	{
		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(client_socket, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);


		SendClientInfo();
#if DEBUG
		switch (client_type)
		{
		case Instance::type::first_client_handler:
		{
			std::cout << "first\n";
		} break;

		case Instance::type::second_client_handler:
		{
			std::cout << "second\n";
		} break;

		default:
			std::cout << "dafault\n";
			break;
		}

#endif
		recieve_thread = (std::thread(&Client::RecieveData, this));

		while (is_connected)
		{
			SendData();
		}
	}

	Client::~Client()
	{
		recieve_thread.join();
		WSACleanup();
		closesocket(client_socket);
	}
}