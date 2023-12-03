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
		MYSQL* connection;
		MYSQL_RES* res;
		int qstate = 0;

	public:
		ChatDB() noexcept;
		~ChatDB() = default;

	public:
		bool InsertUserTo(TCPChat::Client::user_info* uinfo, struct sockaddr_in client_info);
		void DeleteUserFrom();
		void SearchUserIn();
	};
}