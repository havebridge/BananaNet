#pragma once

#include <iostream>
#include <string>

#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define SIZE 128

namespace Hnet
{
	class Server
	{
	private:
		WSAData wsa;
		SOCKET serversocket;
		int port;
		std::string ip;
		struct sockaddr_in info;
		int infolength;
		int recvlength;
		int sendlength;
		char buffer[SIZE];
		std::string message;
	public:
		Server(std::string, int);
		~Server();
	public:
		void Start();
	private:
		void init();
		void recieve();
		void proccess();
		void send();
	};
}