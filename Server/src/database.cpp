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

	bool ChatDB::InsertUserTo(TCPChat::Client::user_info* uinfo, struct sockaddr_in client_info)
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
			mysql_close(&mysql);
			return false;
		}

		return true;
	}
}