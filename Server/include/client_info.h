#pragma once

#include <iostream>
#include <mutex>
#include <vector>


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

		struct user_info
		{
			std::string username;
			std::string login;
			std::string password;
		} uinfo;

		struct user_info_dto
		{
			std::vector<std::string> usernames;
			int client_count = 0;
		};

		struct message_info
		{
			std::string message;
			std::string from;
			std::string to;
		} message;

		struct sockaddr_in client_info;
		SOCKET client_socket;
		bool is_connected = true;

		std::mutex client_handler;

		std::string GetHost() const;
		std::string GetPort() const;

		Client(struct sockaddr_in client_info, SOCKET client_socket, user_info _uinfo)
			:
			client_info(client_info),
			client_socket(client_socket),
			uinfo(_uinfo) {}

		~Client()
		{
			is_connected = false;

			if (client_socket == INVALID_SOCKET)
			{
				return;
			}

			client_socket = INVALID_SOCKET;
			closesocket(client_socket);
		}
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

		bool GetStatus() { return is_connected; }
	};
}