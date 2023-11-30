#pragma once

#include <iostream>

#include <mutex>
#include "../../Core/Core.h"

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
			char username[20];
			char login[20];
			char password[20];

			ConnectionType type;
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
			client_socket(client_socket)
		{
			for (int i = 0; i != 20; ++i)
			{
				uinfo.username[i] = _uinfo.username[i];
				uinfo.login[i] = _uinfo.login[i];
				uinfo.password[i] = _uinfo.password[i];
			}
		}

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