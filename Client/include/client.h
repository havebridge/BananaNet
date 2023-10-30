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
		struct ClientInfoSend
		{
			char login[MAX_LENGTH];
			char password[MAX_LENGTH];

			char ip[MAX_LENGTH];
			char* port;
		};

	private:
		WSAData wsa;

		SOCKET client_socket;

		struct sockaddr_in server_info;
		int server_info_lenght;

		PIP_ADAPTER_INFO client_local_ip;
		std::string client_external_ip;
		struct sockaddr_in client_info;
		int client_info_lenght;

		std::string send_message;
		int send_message_size;
		bool is_connected;

		char* recieve_message;
		int recieve_message_size;
		std::thread recieve_thread;
		std::mutex recieve_mutex;
		std::condition_variable cv;

		Instance::type client_type;
		std::ifstream client_handler_file;

		//std::unique_ptr<ClientInfoSend> client_info_send;
		ClientInfoSend client_info_send;
	private:
		void GetClientExternalIp();
		bool SendClientInfo();
		bool ProcessHandlerFile(const char* file_name);
		void ProcessClientInfo(const char* ip);
		void SendData();
		void RecieveData();

	public:
		explicit Client() noexcept;
		Client(const Client&) = delete;
		Client operator=(const Client&) = delete;
		~Client();

	public:
		bool Connect(std::string ip, int port);
		void Run();
	};
}