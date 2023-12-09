#pragma once 

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
		MYSQL_ROW row;
		int qstate = 0;

	public:
		ChatDB() noexcept;
		~ChatDB() noexcept;

	public:
		bool InsertUser(TCPChat::Client::user_info* uinfo, struct sockaddr_in client_info);
		bool UpdateUserInfo(std::string login);
		bool GetUsers(std::string username, TCPChat::Client::users_info_dto& users);
		void DeleteUsersInfo();
		bool LoadMessageHistory(std::string login);
		void DeleteUser();
	};
}