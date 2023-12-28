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

	void ChatDB::DeleteUsersInfo()
	{
		std::string query_string = "DELETE from chat_use;";

		qstate = mysql_query(&mysql, query_string.c_str());

		if (qstate != 0)
		{
			std::cout << mysql_error(&mysql) << std::endl;
			mysql_free_result(res);
			mysql_close(connection);
		}

		mysql_store_result(&mysql);
	}

	bool ChatDB::InsertUser(TCPChat::Client::user_info* uinfo, struct sockaddr_in client_info)
	{
		std::string query_string = "INSERT INTO chat_user (username, login, password, ip, last_visited_at) VALUE ('";
		query_string += uinfo->username + "','";
		query_string += uinfo->login + "','";
		query_string += uinfo->password + "', INET_ATON('";
		query_string += std::string(inet_ntoa(client_info.sin_addr)) + "'), NOW())";

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
		std::string query_string = "UPDATE chat_user SET last_visited_at = NOW() WHERE login = '";
		query_string += login + "';";

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

	bool ChatDB::GetUsers(std::string username, TCPChat::Client::user_info_dto& data)
	{
		std::string query_string = "SELECT username FROM chat_user WHERE NOT username = '";
		query_string += username + "';";

		qstate = mysql_query(&mysql, query_string.c_str());

		res = mysql_use_result(&mysql);

		if (qstate != 0)
		{
			std::cout << mysql_error(&mysql) << std::endl;
			mysql_close(connection);
			return false;
		}

		int k = 0;
		while (row = mysql_fetch_row(res)) 
		{
			data.usernames.push_back(row[0]);
		}

		mysql_store_result(&mysql);

		return true;
	}
}