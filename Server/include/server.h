#pragma once

#include "database.h"
#include "thread_pool.h"
#include "logger.h"
#include "client_info.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace TCPChat
{
	class Server
	{
	private:
		WSAData wsa;

		SOCKET server_socket;
		std::string ip;
		int port;

		struct sockaddr_in server_info;
		int server_info_lenght;

		bool running;

		std::string send_message;
		std::string recieved_message;
		int send_message_size;
		int recieved_message_size;

		std::vector<std::unique_ptr<Client>> clients;
		std::mutex client_mutex;
		std::condition_variable cv;

		Core::ThreadPool thread_pool;
		std::atomic<bool> restart_loop = false;

		Core::ChatDB db;

		int client_count = 0;

	private:
		void GetClientsInfo();
		int GetButtonType(SOCKET client_socket);
		void GetUserInfo(SOCKET client_socket, Client::user_info& client_info);

		bool HandleSignUp(SOCKET client_socket, sockaddr_in client_info, Client::user_info& client, Client::user_info_dto& client_info_dto);
		bool HandleSignIn(SOCKET client_socket, Client::user_info client_info, Client::user_info_dto client_info_dto);

		bool SearchForClient(Client::user_info* uinfo);

		bool SendClientsInfo(const Client::user_info_dto& uinfo, SOCKET client_socket);
		bool SendMessageToClient(json json_data);
		void SendInfoMessageToClient(SOCKET client_socket, bool is_exist);

		void UpdateSocket(SOCKET new_socket, const std::string login);

		void ClientHandler();
		void ProcessData();

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