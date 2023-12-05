#pragma once 

#include <iostream>
#include <string>
#include <ctime>
#include <mysql.h>

#include "../include/client_info.h"

namespace Core
{
	class ChatDB
	{
	private:
		MYSQL mysql;
		MYSQL* connection = nullptr;
		MYSQL_RES* res = nullptr;
		int qstate = 0;

	public:
		ChatDB() noexcept;
		~ChatDB() noexcept;

	public:
		bool InsertUser(TCPChat::Client::user_info* uinfo, struct sockaddr_in client_info);
		bool UpdateUserInfo(std::string login);
		void DeleteUser();
	};
}