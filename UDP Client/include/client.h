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
		SOCKET client_socket;
		struct sockaddr_in client_info;
		int client_info_lenght;
		char* message;
		int message_size;

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