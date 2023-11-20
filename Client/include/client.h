#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "../../Core/Core.h"


namespace UDPChat
{
	class Client
	{
	private:
		/* I'm not sure if I need it yet
		PIP_ADAPTER_INFO client_local_ip;*/

		/* MySQL is the way
		Instance::type client_type;
		std::ifstream client_handler_file; */

		WSAData wsa;

		SOCKET client_socket;

		struct sockaddr_in server_info;
		int server_info_lenght;

		std::string client_external_ip;
		struct sockaddr_in client_info;
		int client_info_lenght;

		std::string login;
		std::string password;
		std::string send_message;
		std::string recieved_message;
		int send_message_size;
		int recieved_message_size;

		bool is_connected = false;
	private:
		void GetClientExternalIp();
		//bool ProcessHandlerFile(const char* file_name);
		void SendData();
		void RecieveData();

	public:
		explicit Client() noexcept;
		Client(const Client&) = delete;
		Client operator=(const Client&) = delete;
		~Client();

	public:
		bool Connect(std::string ip, int port);
		void Disconnect();
		void Run();
	};
}