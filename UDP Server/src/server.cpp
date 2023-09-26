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

	bool Server::ProcessFile(Instance::type client_handler)
	{
		client_handler_file.open("C:/Users/meylor/source/repos/hnet/handler/file_handler.txt");

		if (client_handler_file.is_open())
		{
			client_handler_file << static_cast<int>(client_handler);
			client_handler_file.close();
			return true;
		}
		else
		{
			std::cerr << "Unable to open the file\n";
			return false;
		}
	}

	void Server::ClientsHandler()
	{
		if (is_first_client_connected == false)
		{
			char* ip = new char[INET_ADDRSTRLEN + 1];
			ip[INET_ADDRSTRLEN] = '\0';

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
			Instance::client_handler = Instance::type::first_client_handler;
			ProcessFile(Instance::client_handler);
		}
		else
		{
			char* ip = new char[INET_ADDRSTRLEN + 1];
			ip[INET_ADDRSTRLEN] = '\0';

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
			Instance::client_handler = Instance::type::second_client_handler;
			ProcessFile(Instance::client_handler);
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

		int client = 0;

		if (recvfrom(server_socket, (char*)&client, sizeof(int), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			perror("recvfrom message");
			return;
		}

		std::cout << "message size recv: " << message_size;
		std::cout << "\nmessage recv: " << message;
		std::cout << "\client handler recv: " << client;
		std::cout << '\n';



		if (sendto(server_socket, (char*)&message_size, sizeof(int), 0, (sockaddr*)&first_client_info, first_client_info_lenght) <= 0)
		{
			perror("recvfrom message size");
			return;
		}

		if (sendto(server_socket, message, message_size, 0, (sockaddr*)&first_client_info, first_client_info_lenght) <= 0)
		{
			perror("recvfrom message");
			return;
		}

		if (sendto(server_socket, (char*)&client, sizeof(int), 0, (sockaddr*)&first_client_info, first_client_info_lenght) <= 0)
		{
			perror("recvfrom message");
			return;
		}

		std::cout << "message size send: " << message_size;
		std::cout << "\nmessage send: " << message;
		std::cout << "\client handler send: " << client;
		std::cout << '\n';

		delete[] message;
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


	void Server::Stop()
	{
		closesocket(server_socket);
	}

	Server::~Server()
	{
		WSACleanup();
		closesocket(server_socket);
	}
}