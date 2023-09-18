#pragma once

#include <iostream>
#include <string>

#include <WinSock2.h>


#define SIZE 128

namespace UDPChat
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

	private:
		void Recieve();
		void Process();
		void Send();

	public:
		explicit Server(std::string ip, int port);
		Server(const Server&) = delete;
		Server operator=(const Server&) = delete;
		~Server();

	public:
		bool Init();

		void Start();
		void Stop();
	};
}