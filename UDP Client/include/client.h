#pragma once

#include <iostream>
#include <string>

#include "../../Core.h"


namespace UDPChat
{
	class Client
	{
	private:
		WSAData wsa;
		SOCKET client_socket;
		struct sockaddr_in client_info;
		int client_info_lenght;

		std::string send_message;
		int send_message_size;

		char* recv_message;
		int recv_message_size;

		bool is_connected;

		Instance::type client_type;
		
	private:
		void SendInfo();
		void SendMSG();
		void RecvMSG();

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