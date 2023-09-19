#pragma once

#include <iostream>
#include <string>

#include <WinSock2.h>
#include <Windows.h>


#define	MAX_NUM_CLIENTS 2

namespace UDPChat
{
	class Server
	{
	private:
		WSAData wsa;

		SOCKET server_socket;
		int port;
		std::string ip;

		struct sockaddr_in server_info;
		int server_info_lenght;

		char* message;
		int message_size;

		SOCKET first_client;
		char first_client_ip[10];
		int first_client_port[10];

		SOCKET second_client;
		char* second_client_ip;
		int second_client_port;

	private:
		void FirstClientHandler();

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