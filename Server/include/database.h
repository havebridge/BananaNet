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

	private:
		int GetIDByUsername(const std::string& username);
		int GetIDByLogin(const std::string& login);

	public:
		ChatDB() noexcept;
		~ChatDB() noexcept;

	public:
		bool InsertUser(TCPChat::Client::user_info* uinfo, struct sockaddr_in client_info);
		bool UpdateUserInfo(const std::string& login);
		bool GetUsers(const std::string& username, TCPChat::Client::user_info_dto& users);
		void DeleteUsersInfo();
		bool AddMessage(const std::string& message, const std::string& from, const std::string& to);
		bool LoadMessageHistory(const std::string& login);
		void DeleteUser();
	};
}