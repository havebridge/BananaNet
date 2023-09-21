#pragma once

#include <iostream>
#include <string>

#include "../../Core.h"


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

		struct sockaddr_in first_client_info;
		int first_client_info_lenght;

		struct sockaddr_in second_client_info;
		int second_client_info_lenght;

		bool is_first_client_connected;
		bool is_second_client_connected;

	private:
		void ClientsHandler();
		void ProcessMessage();

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