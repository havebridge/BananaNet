#include "../include/server.h"

#pragma warning(disable: 4996)

namespace Hnet
{
	Server::Server(std::string ip, int port)
		:
		wsa{ 0 },
		serversocket(INVALID_SOCKET),
		ip(ip),
		port(port),
		info{ 0 },
		infolength(sizeof(info)),
		recvlength(0),
		sendlength(0) {}


	void Server::setup()
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) == -1)
		{
			WSACleanup();
			closesocket(serversocket);
			fprintf(stderr, "WSAStartup failed.\n");
			exit(EXIT_FAILURE);
		}

		if ((serversocket = socket(PF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
		{
			WSACleanup();
			closesocket(serversocket);
			perror("socket");
			exit(EXIT_FAILURE);
		}

		char yes = '1';

		if ((setsockopt(serversocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1))
		{
			WSACleanup();
			closesocket(serversocket);
			perror("setsockport");
			exit(EXIT_FAILURE);
		}

		info.sin_family = AF_INET;
		info.sin_port = htons(port);
		info.sin_addr.s_addr = inet_addr(ip.c_str());

		ZeroMemory(info.sin_zero, 8);

		if ((bind(serversocket, (struct sockaddr*) & info, infolength) == -1))
		{
			WSACleanup();
			closesocket(serversocket);
			perror("bind");
			exit(EXIT_FAILURE);
		}


		std::cout << "UDP Server started at:" << inet_ntoa(info.sin_addr) << ":" << htons(info.sin_port) << '\n';
	}


	void Server::Start()
	{
		setup();

	
	}


	Server::~Server()
	{
		WSACleanup();
		closesocket(serversocket);
	}
}