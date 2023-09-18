#pragma once

#include <iostream>
#include <string>

#include <WinSock2.h>


#define SIZE 128

namespace UDPChat
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

	private:
		void Init();
		void Send();
		void Recieve();
		void Process();

	public:
		explicit Client();
		Client(const Client&) = delete;
		Client operator=(const Client&) = delete;
		~Client();

	public:
		bool Connect(std::string ip, int port);
		void Run();
	};
}