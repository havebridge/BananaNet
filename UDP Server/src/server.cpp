#include "../include/server.h"

#pragma warning(disable: 4996)


namespace UDPChat
{
	Server::Server(std::string ip, int port)
		:
		wsa{ 0 },
		serversocket(INVALID_SOCKET),
		ip(std::move(ip)),
		port(port),
		info{ 0 },
		infolength(sizeof(info)),
		recvlength(0),
		sendlength(0) {}


	bool Server::Init()
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) == -1)
		{
			perror("WSAStartup");
			return false;
		}

		if ((serversocket = socket(PF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
		{
			perror("socket");
			return false;
		}

		char yes = '1';

		if ((setsockopt(serversocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1))
		{
			perror("setsockport");
			return false;
		}

		info.sin_family = AF_INET;
		info.sin_port = htons(port);
		info.sin_addr.s_addr = inet_addr(ip.c_str());

		ZeroMemory(info.sin_zero, 8);

		if ((bind(serversocket, (struct sockaddr*) & info, infolength) == -1))
		{
			perror("bind");
			return false;
		}


		std::cout << "UDP Server started at:" << inet_ntoa(info.sin_addr) << ":" << htons(info.sin_port) << '\n';

		return true;
	}

	void Server::Start()
	{
		for (;;)
		{
			Recieve();
			Process();
			Send();
		}
	}

	void Server::Recieve()
	{
		if ((recvlength = recvfrom(serversocket, buffer, SIZE, 0, (struct sockaddr*) & info, &infolength)) == 0)
		{
			perror("recvfrom");
			return;
		}
	}

	void Server::Process()
	{
		std::cout << "Got the packet from:" << inet_ntoa(info.sin_addr) << ":" << htons(info.sin_port) << '\n';
		std::cout << "Data:";
	
		for (int i = 0; i != recvlength; ++i)
		{
			std::cout << buffer[i];
		}

		std::cout << '\n';
	}

	void Server::Send()
	{
		if ((sendlength = sendto(serversocket, buffer, recvlength, 0, (struct sockaddr*) & info, infolength)) == 0)
		{
			perror("send");
			return;
		}
	}

	Server::~Server()
	{
		WSACleanup();
		closesocket(serversocket);
	}
}