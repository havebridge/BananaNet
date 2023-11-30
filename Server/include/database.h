#pragma once 

#include <iostream>
#include <string>
#include <mysql.h>

namespace Core
{
	class ChatDB
	{
	private:
		MYSQL mysql;
		MYSQL* connection;
		MYSQL_RES res;
		MYSQL_ROW row;
		int qstate = 0;

	public:
		ChatDB() noexcept;
		~ChatDB() = default;

	public:
		bool InsertUserTo();
		void DeleteUserFrom();
		void SearchUserIn();
	};
}