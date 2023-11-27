#include "../include/client.h"

#pragma warning(disable: 4996)

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

namespace UDPChat
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
		
		std::cout << "Client info\n";
		std::cout << "external ip: " << client_external_ip << " port: " << client_info.sin_port << '\n';

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
		is_connected = false;
	}


	void Client::Run()
	{


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
		//recieve_thread.join();
		closesocket(client_socket);
		WSACleanup();
	}
}