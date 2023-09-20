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
		second_client_info{ 0 } {}


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

	void Server::FirstClientHandler()
	{
		char* ip = new char[sizeof(first_client_info.sin_addr) + 1];

		if (recvfrom(server_socket, ip, 10, 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recvfrom first client ip");
			return;
		}

		std::cout << ip << '\n';

		first_client_info.sin_addr.s_addr = (u_short)ip;
		inet_pton(AF_INET, ip, &(first_client_info.sin_addr));

		std::cout << inet_ntoa(first_client_info.sin_addr) << '\n';

		int port = 0;

		if (recvfrom(server_socket, (char*)&port, sizeof(int), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recvfrom first client port");
			return;
		}

		std::cout << port << '\n';

		first_client_info.sin_port = port;

		std::cout << std::to_string(first_client_info.sin_port) << '\n';

		//delete [] ip;
	}

	void Server::SecondClientHandler()
	{
		char* ip = new char[sizeof(second_client_info.sin_addr) + 1];

		if (recvfrom(server_socket, ip, 10, 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recvfrom first client ip");
			return;
		}

		std::cout << ip << '\n';

		second_client_info.sin_addr.s_addr = (u_short)ip;
		inet_pton(AF_INET, ip, &(second_client_info.sin_addr));

		std::cout << inet_ntoa(second_client_info.sin_addr) << '\n';

		int port = 0;

		if (recvfrom(server_socket, (char*)&port, sizeof(int), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recvfrom first client port");
			return;
		}

		std::cout << port << '\n';

		second_client_info.sin_port = port;

		std::cout << std::to_string(second_client_info.sin_port) << '\n';

		//delete [] ip;
	}

	void Server::Start()
	{
		//FirstClientHandler();

		//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);

		FirstClientHandler();
		SecondClientHandler();

		/*if (recvfrom(server_socket, reinterpret_cast<char*>(first_client_port), sizeof(int), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			perror("recvfrom first client port");
			return;
		}*/
	}


	Server::~Server()
	{
		WSACleanup();
		closesocket(server_socket);
	}
}