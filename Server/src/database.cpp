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
		std::string query_string = "DELETE from chat_user;";

		qstate = mysql_query(&mysql, query_string.c_str());

		if (qstate != 0)
		{
			std::cout << mysql_error(&mysql) << std::endl;
			mysql_free_result(res);
			mysql_close(connection);
		}

		//mysql_store_result(&mysql);
	}

	bool ChatDB::InsertUser(TCPChat::Client::user_info* uinfo, struct sockaddr_in client_info)
	{
		std::string query_string = "INSERT INTO chat_user (username, login, password, ip, last_visited_at) VALUES (?, ?, ?, ?, NOW())";

		MYSQL_STMT* stmt = mysql_stmt_init(&mysql);

		if (!stmt) 
		{
			std::cout << "mysql_stmt_init(), out of memory\n";
			return false;
		}

		if (mysql_stmt_prepare(stmt, query_string.c_str(), query_string.length()) != 0)
		{
			std::cout << "mysql_stmt_prepare(), INSERT failed\n";
			return false;
		}

		MYSQL_BIND bind[5];
		memset(bind, 0, sizeof(bind));

		bind[0].buffer_type = MYSQL_TYPE_STRING;
		bind[0].buffer = const_cast<char*>(uinfo->username.c_str());
		bind[0].buffer_length = uinfo->username.length();
		bind[0].is_null = 0;

		bind[1].buffer_type = MYSQL_TYPE_STRING;
		bind[1].buffer = const_cast<char*>(uinfo->login.c_str());
		bind[1].buffer_length = uinfo->login.length();
		bind[1].is_null = 0;

		bind[2].buffer_type = MYSQL_TYPE_STRING;
		bind[2].buffer = const_cast<char*>(uinfo->password.c_str());
		bind[2].buffer_length = uinfo->password.length();
		bind[2].is_null = 0;

		std::string ip = std::string(inet_ntoa(client_info.sin_addr));
		bind[3].buffer_type = MYSQL_TYPE_STRING;
		bind[3].buffer = const_cast<char*>(ip.c_str());
		bind[3].buffer_length = ip.length();
		bind[3].is_null = 0;

		bind[4].buffer_type = MYSQL_TYPE_DATETIME;
		bind[4].is_null = 0;

		if (mysql_stmt_bind_param(stmt, bind) != 0) {
			std::cout << "mysql_stmt_bind_param() failed\n";
			return false;
		}

		if (mysql_stmt_execute(stmt) != 0) {
			std::cout << "mysql_stmt_execute(), INSERT failed\n";
			return false;
		}

		mysql_stmt_close(stmt);

		return true;
	}

	bool ChatDB::UpdateUserInfo(std::string login)
	{
		std::string query_string = "UPDATE chat_user SET last_visited_at = NOW() WHERE login = ?";

		MYSQL_STMT* stmt = mysql_stmt_init(&mysql);

		if (!stmt)
		{
			std::cout << "mysql_stmt_init(), out of memory\n";
			return false;
		}

		if (mysql_stmt_prepare(stmt, query_string.c_str(), query_string.length()) != 0) 
		{
			std::cout << "mysql_stmt_prepare(), UPDATE failed\n";
			return false;
		}

		MYSQL_BIND bind[1];
		memset(bind, 0, sizeof(bind));

		bind[0].buffer_type = MYSQL_TYPE_STRING;
		bind[0].buffer = const_cast<char*>(login.c_str());
		bind[0].buffer_length = login.length();
		bind[0].is_null = 0;

		if (mysql_stmt_bind_param(stmt, bind) != 0)
		{
			std::cout << "mysql_stmt_bind_param() failed\n";
			return false;
		}

		if (mysql_stmt_execute(stmt) != 0) 
		{
			std::cout << "mysql_stmt_execute(), UPDATE failed\n";
			return false;
		}

		mysql_stmt_close(stmt);

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

	int ChatDB::GetIDByUsername(std::string username)
	{
		int user_id = 0;

		std::string query_string = "SELECT id FROM chat_user WHERE username = '";
		query_string += username + "';";


		qstate = mysql_query(&mysql, query_string.c_str());

		if (qstate != 0)
		{
			std::cout << mysql_error(&mysql) << std::endl;
			mysql_close(connection);
			return 0;
		}

		res = mysql_store_result(&mysql);

		if ((row = mysql_fetch_row(res)))
		{
			user_id = atoi(row[0]);
		}

		return user_id;
	}


	int ChatDB::GetIDByLogin(std::string login)
	{
		int user_id = 0;

		std::string query_string = "SELECT id FROM chat_user WHERE login = '";
		query_string += login + "';";


		qstate = mysql_query(&mysql, query_string.c_str());

		if (qstate != 0)
		{
			std::cout << mysql_error(&mysql) << std::endl;
			mysql_close(connection);
			return 0;
		}

		res = mysql_store_result(&mysql);

		if ((row = mysql_fetch_row(res)))
		{
			user_id = atoi(row[0]);
		}

		return user_id;
	}

	bool ChatDB::AddMessage(const std::string message, const std::string from, const std::string to)
	{
		std::cout << "ADD MESSAGE FUNC\n";

		int user_id_form = GetIDByLogin(from);
		int user_id_to= GetIDByUsername(to);

		std::string query_string = "INSERT INTO chat_line (sender_id, receiver_id, message_text) VALUES ('";
		query_string += std::to_string(user_id_form) + "','";
		query_string += std::to_string(user_id_to) + "','";
		query_string += message + "');";
		
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