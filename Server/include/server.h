#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

#include "../../Core/Core.h"
#include "thread_pool.h"
#include "logger.h"

#define	MAX_NUM_CLIENTS 2

namespace UDPChat
{
	class Server
	{
	private:
		struct Client
		{
			//friend class Server;

			std::string login;
			std::string password;

			struct sockaddr_in client_info;
			SOCKET client_socket;
			bool is_connected = true;

			//std::mutex client_handler;

			std::string GetHost() const;
			std::string GetPort() const;

			Client(struct sockaddr_in client_info, SOCKET client_socket)
				:
				client_info(client_info),
				client_socket(client_socket) {}

			~Client()
			{
				if (client_socket == INVALID_SOCKET)
				{
					return;
				}

				client_socket = INVALID_SOCKET;
				closesocket(client_socket);
			}
		};

	private:
		WSAData wsa;

		SOCKET server_socket;
		int port;
		std::string ip;

		struct sockaddr_in server_info;
		int server_info_lenght;
		std::condition_variable message_cv;
		bool running;

		char* recieved_message;
		int recieved_message_size;

		std::string send_message;
		int send_message_size;

		struct sockaddr_in in;	

		std::vector<std::unique_ptr<Client>> clients;
		std::mutex client_handler;
		std::condition_variable cv;

		Core::ThreadPool thread_pool;
		std::mutex message_mutex;

	private:
		void ClientsHandler();
		void ProcessMessage();
		bool ProcessFile(Instance::type client_handler);
		void JoinLoop() { thread_pool.Join(); }

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