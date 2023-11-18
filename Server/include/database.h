#pragma once 

#include <iostream>
#include <string>
#include <mysql.h>
namespace Core
{
	class ChatDB
	{
	private:
		MYSQL* connect = mysql_init(0);
		MYSQL_ROW row;
		MYSQL_RES* res;
		int qstate;
	public:
		ChatDB();
		~ChatDB() = default;
	};
}