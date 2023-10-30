#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "../../Core/Core.h"
#include "logger.h"

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

		char* recieved_message;
		int recieved_message_size;

		std::string send_message;
		int send_message_size;

		struct sockaddr_in first_client_info;
		struct sockaddr_in second_client_info;
		struct sockaddr_in in;

		bool is_first_client_connected;
		bool is_second_client_connected;

		std::ofstream client_handler_file;	

	private:
		void ClientsHandler();
		void ProcessMessage();
		bool ProcessFile(Instance::type client_handler);

	public:
		explicit Server(std::string ip, int port) noexcept;
		Server(const Server&) = delete;
		Server operator=(const Server&) = delete;
		~Server();

	public:
		bool Init();

		void Start();
		void Stop();
	};
}