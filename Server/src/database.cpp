#include "../include/database.h"

namespace Core
{
	ChatDB::ChatDB() noexcept
	{
		mysql_init(&mysql);
		connection = mysql_real_connect(&mysql, "localhost", "root", "password", "hnet", 3306, NULL, 0);

		if (connection == NULL)
		{
			std::cout << mysql_error(&mysql) << std::endl;
			mysql_close(&mysql);
		}
	}

	ChatDB::~ChatDB() noexcept
	{
		mysql_free_result(res);
		mysql_close(connection);
	}

	bool ChatDB::InsertUser(TCPChat::Client::user_info* uinfo, struct sockaddr_in client_info)
	{
		std::string query_string = "INSERT INTO chat_user (username, login, password, ip, last_visited_at) VALUE (";
		query_string += "'" + std::string(uinfo->username) + "',";
		query_string += "'" + std::string(uinfo->login) + "',";
		query_string += "'" + std::string(uinfo->password) + "',";
		query_string += "INET_ATON('" + std::string(inet_ntoa(client_info.sin_addr)) + "'),";
		query_string += "NOW()";
		query_string += ")";

		qstate = mysql_query(&mysql, query_string.c_str());

		if (qstate != 0)
		{
			std::cout << mysql_error(&mysql) << std::endl;
			mysql_close(connection);
			return false;
		}

		mysql_store_result(&mysql);

		return true;
	}

	bool ChatDB::UpdateUserInfo(std::string login)
	{
		std::string query_string = "UPDATE chat_user SET last_visited_at = NOW() WHERE login = ";
		query_string += "'" + login + "';";

		qstate = mysql_query(&mysql, query_string.c_str());

		if (qstate != 0)
		{
			std::cout << mysql_error(&mysql) << std::endl;
			mysql_close(connection);
			return false;
		}

		mysql_store_result(&mysql);

		return true;
	}
}