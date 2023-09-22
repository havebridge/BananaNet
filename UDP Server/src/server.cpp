#include "../include/server.h"

#pragma warning(disable: 4996)


namespace UDPChat
{
	Server::Server(std::string ip, int port)
		:
		wsa{ 0 },
		server_socket(INVALID_SOCKET),
		ip(std::move(ip)),
		port(port),
		server_info{ 0 },
		server_info_lenght(sizeof(server_info)),
		message_size(0),
		first_client_info{ 0 },
		second_client_info{ 0 },
		is_first_client_connected(false),
		is_second_client_connected(false) {}


	bool Server::Init()
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) == -1)
		{
			perror("WSAStartup");
			return false;
		}

		if ((server_socket = socket(PF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
		{
			perror("socket");
			return false;
		}

		char yes = '1';

		if ((setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1))
		{
			perror("setsockport");
			return false;
		}

		server_info.sin_family = AF_INET;
		server_info.sin_port = htons(port);
		server_info.sin_addr.s_addr = inet_addr(ip.c_str());

		ZeroMemory(server_info.sin_zero, 8);

		if (bind(server_socket, (const sockaddr*)&server_info, server_info_lenght) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("bind");
			return false;
		}

		std::cout << "UDP Server started at:" << inet_ntoa(server_info.sin_addr) << ":" << htons(server_info.sin_port) << '\n';

		return true;
	}

	void Server::ClientsHandler()
	{
		if (is_first_client_connected == false)
		{
			char* ip = new char[INET_ADDRSTRLEN + 1];

			if (recvfrom(server_socket, ip, INET_ADDRSTRLEN, 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom first client ip");
				return;
			}

			first_client_info.sin_addr.s_addr = reinterpret_cast<u_short>(ip);
			inet_pton(AF_INET, ip, &(first_client_info.sin_addr));

			int port = 0;

			if (recvfrom(server_socket, (char*)&port, sizeof(int), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom first client port");
				return;
			}

			first_client_info.sin_port = port;
			first_client_info.sin_family = AF_INET;

			ZeroMemory(first_client_info.sin_zero, 8);

			std::cout << "First client is handled\n";
			std::cout << "IP: " << inet_ntoa(first_client_info.sin_addr) << " PORT: " << std::to_string(first_client_info.sin_port) << '\n';
			delete[] ip;

			is_first_client_connected = true;
		}
		else
		{
			char* ip = new char[INET_ADDRSTRLEN + 1];

			if (recvfrom(server_socket, ip, INET_ADDRSTRLEN, 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom first client ip");
				return;
			}

			second_client_info.sin_addr.s_addr = reinterpret_cast<u_short>(ip);
			inet_pton(AF_INET, ip, &(second_client_info.sin_addr));

			int port = 0;

			if (recvfrom(server_socket, (char*)&port, sizeof(int), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom first client port");
				return;
			}

			second_client_info.sin_port = port;
			second_client_info.sin_family = AF_INET;

			ZeroMemory(second_client_info.sin_zero, 8);

			std::cout << "Second client is handled\n";
			std::cout << "IP: " << inet_ntoa(second_client_info.sin_addr) << " PORT: " << std::to_string(second_client_info.sin_port) << '\n';
			delete[] ip;

			is_second_client_connected = true;
		}
	}


	void Server::ProcessMessage()
	{
		message_size = 0;

		if (recvfrom(server_socket, (char*)&message_size, sizeof(int), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			perror("recvfrom message size");
			return;
		}

		message = new char[message_size + 1];
		message[message_size] = '\0';

		if (recvfrom(server_socket, message, message_size, 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			perror("recvfrom message");
			return;
		}

		std::cout << "message size recv: " << message_size;
		std::cout << "\nmessage recv: " << message;
		std::cout << '\n';
	}

	void Server::Start()
	{
		while (is_first_client_connected == false || is_second_client_connected == false)
		{
			ClientsHandler();
		}

		while (is_first_client_connected && is_second_client_connected)
		{
			ProcessMessage();
		}
	}


	Server::~Server()
	{
		WSACleanup();
		closesocket(server_socket);
	}
}