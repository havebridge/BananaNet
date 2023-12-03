#include "../include/database.h"

#include "../../Core/core.h"

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
		uint32_t ip = client_info.sin_addr.s_addr;
		std::string formated_ip = std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
			std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
			std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
			std::to_string(int(reinterpret_cast<char*>(&ip)[3]));

		std::string query_string = "INSERT INTO chat_user (username, login, password, ip, last_visited_at) VALUE (";
		query_string += std::to_string(*uinfo->username) + ",";
		query_string += std::to_string(*uinfo->login) + ",";
		query_string += std::to_string(*uinfo->password);
		query_string += formated_ip + ",";
		//query_string += formated_ip + ",";
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