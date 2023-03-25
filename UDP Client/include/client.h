#pragma once

#include <iostream>
#include <string>

#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define SIZE 128

namespace Hnet
{
	class Client
	{
	private:
		WSAData wsa;
		SOCKET clientsocket;
		int port;
		std::string ip;
		struct sockaddr_in info;
		int infolength;
		int recvlength;
		int sendlength;
		char buffer[SIZE];
		std::string message;
	public:
		Client(std::string, int);
		~Client();
	public:
		void Connect();
	private:
		void init();
		void send();
		void recieve();
		void proccess();
	};
}