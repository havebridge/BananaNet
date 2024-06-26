#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <format>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "Core.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace TCPChat
{
	class Client
	{
	public:
		enum ButtonType : int8_t
		{
			SIGN_UP = 1,
			SIGN_IN,
			BACK
		};

	private:
		struct user_info_dto
		{
			std::vector<std::string> usernames;
			int client_count = 0;
		} uinfo_dto;

		struct message_info
		{
			std::string message;
			std::string from;
			std::string to;
		} message;

		WSAData wsa;

		SOCKET client_socket;

		struct sockaddr_in server_info;
		int server_info_lenght;

		struct sockaddr_in client_info;
		int client_info_lenght;

		std::string client_external_ip;

		std::string send_message;
		std::string recieved_message;
		int send_message_size;
		int recieved_message_size;

		std::thread recv_thread;

		bool is_connected = false;

	private:
		void GetClientExternalIp();

	public:
		friend std::ostream& operator<<(std::ostream& stream, const ButtonType cType)
		{
			std::string result;
#define PRINT(a) result = #a;
			switch (cType)
			{
			case SIGN_UP: PRINT(SIGN_UP); break;
			case SIGN_IN: PRINT(SIGN_IN); break;
			}

			return stream << result;
		}

	public:
		explicit Client() noexcept;
		Client(const Client&) = delete;
		~Client();

		user_info_dto GetDto() { return uinfo_dto; }
		message_info GetMessageInfo() { return message; }
		void SetMessageInfoFrom(std::string from) { message.from = from; }

	public:
		bool Connect(const std::string& ip, int port);

		bool SendUserInfoSignUp(const std::string& username, const std::string& login, const std::string& password);
		bool SendUserInfoSignIn(const std::string& login, const std::string& password);
		void SendButtonInfo(int type_button);
		void SendMessageText(const std::string& message, const std::string& from, const std::string& to);

		void RecieveUsersInfo();
		bool RecieveMessageText(std::string& message);

		void Disconnect();
		void Run();

	public:
		static Client& GetInstance()
		{
			static Client instance;
			return instance;
		}
	};
}