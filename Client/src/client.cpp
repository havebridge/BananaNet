#include "../include/client.h"

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

	void Client::ProcessClientInfo(const char ip[])
	{
		std::cout << "Enter the login: ";
		std::cin >> *client_info_send.login;

		std::cout << "Enter the password: ";
		std::cin >> *client_info_send.password;

		for (int i = *client_info_send.ip; i <= 12; i++)
		{
			client_info_send.ip[i] = ip[i];
		}
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
		ProcessClientInfo(client_external_ip.c_str());
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
		GetClientExternalIp();

		std::cout << "The ip is: " << client_info_send.ip;
		std::cout << "The login is: " << client_info_send.login;
		std::cout << "The password is: " << client_info_send.password;

		/*if (sendto(client_socket, (char*)&is_connected, sizeof(bool), 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
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
			inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));*/

		is_connected = true;

		//cv.notify_one();
		//ProcessHandlerFile("../handler/file_handler.txt");
		return is_connected;
	}

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


	void Client::Run()
	{
		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(client_socket, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);

		SendClientInfo();

		//recieve_thread = (std::thread(&Client::RecieveData, this));

		//while (is_connected)
		//{
		//	SendData();
		//}
	}

	Client::~Client()
	{
		recieve_thread.join();
		WSACleanup();
		closesocket(client_socket);
	}
}