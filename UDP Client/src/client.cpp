#include "../include/client.h"

#pragma warning(disable: 4996)


namespace UDPChat
{
	Client::Client()
		:
		wsa{ 0 },
		clientsocket(INVALID_SOCKET),
		info{ 0 },
		infolength(sizeof(info)),
		recvlength(0),
		sendlength(0) {}


	bool Client::Connect(std::string ip, int port)
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) == -1)
		{
			perror("WSAStartup");
			return false;
		}

		if ((clientsocket = socket(PF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
		{
			perror("socket");
			return false;
		}

		char yes = '1';

		if ((setsockopt(clientsocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1))
		{
			perror("setsockport");
			return false;
		}

		info.sin_family = AF_INET;
		info.sin_port = htons(port);
		info.sin_addr.s_addr = inet_addr(ip.c_str());

		ZeroMemory(info.sin_zero, 8);

		return true;
	}

	void Client::Run()
	{
		for (;;)
		{
			Send();
			Recieve();
			Process();
		}
	}


	void Client::Send()
	{
		std::cout << "Enter a message: ";
		std::getline(std::cin, message);
	

		if ((sendlength = sendto(clientsocket, message.c_str(), message.size(), 0, (struct sockaddr*) & info, infolength)) == 0)
		{
			perror("send");
			return;
		}
	}

	void Client::Recieve()
	{
		if ((recvlength = recvfrom(clientsocket, buffer, SIZE, 0, (struct sockaddr*) & info, &infolength)) == 0)
		{
			perror("recvfrom");
			return;
		}
	}


	void Client::Process()
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