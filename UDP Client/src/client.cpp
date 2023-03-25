#include "../include/client.h"

#pragma warning(disable: 4996)


namespace Hnet
{
	Client::Client(std::string ip, int port)
		:
		wsa{ 0 },
		clientsocket(INVALID_SOCKET),
		port(port),
		ip(ip),
		info{ 0 },
		infolength(sizeof(info)),
		recvlength(0),
		sendlength(0) {}

	void Client::init()
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) == -1)
		{
			WSACleanup();
			closesocket(clientsocket);
			fprintf(stderr, "WSAStartup failed.\n");
			exit(EXIT_FAILURE);
		}

		if ((clientsocket = socket(PF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
		{
			WSACleanup();
			closesocket(clientsocket);
			perror("socket");
			exit(EXIT_FAILURE);
		}

		char yes = '1';

		if ((setsockopt(clientsocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1))
		{
			WSACleanup();
			closesocket(clientsocket);
			perror("setsockport");
			exit(EXIT_FAILURE);
		}

		info.sin_family = AF_INET;
		info.sin_port = htons(port);
		info.sin_addr.s_addr = inet_addr(ip.c_str());

		ZeroMemory(info.sin_zero, 8);
	}


	void Client::Connect()
	{
		init();

		for (;;)
		{
			send();
			recieve();
			proccess();
		}
	}


	void Client::send()
	{
		std::cout << "Enter a message: ";
		std::getline(std::cin, message);
	

		if ((sendlength = sendto(clientsocket, message.c_str(), message.size(), 0, (struct sockaddr*) & info, infolength)) == 0)
		{
			WSACleanup();
			closesocket(clientsocket);
			perror("send");
			exit(EXIT_FAILURE);
		}
	}

	void Client::recieve()
	{
		if ((recvlength = recvfrom(clientsocket, buffer, SIZE, 0, (struct sockaddr*) & info, &infolength)) == 0)
		{
			WSACleanup();
			closesocket(clientsocket);
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}
	}


	void Client::proccess()
	{
		std::cout << "Got the packet from:" << inet_ntoa(info.sin_addr) << ":" << htons(info.sin_port) << '\n';
		std::cout << "Data:";

		for (int i = 0; i != recvlength; ++i)
		{
			std::cout << buffer[i];
		}
		std::cout << '\n';
	}


	Client::~Client()
	{
		WSACleanup();
		closesocket(clientsocket);
	}
}	