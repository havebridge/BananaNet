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
		first_client(INVALID_SOCKET),
		second_client(INVALID_SOCKET) {}


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
		/*if (recvfrom(server_socket, (char*)message_size, sizeof(char), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			perror("recvfrom message size");
			return;
		}

		if (recvfrom(server_socket, message, message_size, 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			perror("recvfrom message");
			return;
		}*/

		/*if (sendto(server_socket, "1", 1, 0, (sockaddr*)&server_info, server_info_lenght) <= 0)
		{
			perror("send");
			return;
		}*/
	}

	void Server::Start()
	{
		//FirstClientHandler();

		//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);

		if (recvfrom(server_socket, first_client_ip, 10, 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recvfrom first client ip");
			return;
		}

		if (recvfrom(server_socket, (char*)first_client_port, 10, 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
		{
			perror("recvfrom first client port");
			return;
		}

		
		
	}


	Server::~Server()
	{
		WSACleanup();
		closesocket(server_socket);
	}
}