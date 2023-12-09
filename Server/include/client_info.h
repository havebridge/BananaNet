#pragma once

#include <iostream>
#include <mutex>
#include <vector>

#define USER_DATA_SIZE 20

namespace TCPChat
{
	class Client
	{
	public:
		enum ConnectionType : int8_t
		{
			SIGN_UP = 1,
			SIGN_IN
		};

		struct user_info
		{
			char username[USER_DATA_SIZE];
			char login[USER_DATA_SIZE];
			char password[USER_DATA_SIZE];

			ConnectionType type;
		};

		struct users_info_dto
		{
			std::vector<std::string> usernames;
			SOCKET client_socket;
			int client_count;
		};

		user_info uinfo;

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
		friend std::ostream& operator<<(std::ostream& stream, const ConnectionType cType)
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
	};
}