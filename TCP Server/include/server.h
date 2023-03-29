#pragma once

#include <iostream>
#include <string>

#ifdef _WIN32
#include <WinSock2.h>
#endif


#include "thread_pool.h"


#pragma comment(lib, "Ws2_32.lib")

#define RELEASE 0
#define DEBUG 1

#define SIZE 128

namespace Hnet
{
	class Server
	{
	private:
		WSAData wsa;
		SOCKET serversocket;
		std::string ip;
		int port;
		struct sockaddr_in info;
		int infolength;
		int recvlength;
		int sendlength;
		char buffer[SIZE];
	public:
		Server(std::string, int);
		~Server();
	public:
		void Start();
	private:
		void setup();
	};
}