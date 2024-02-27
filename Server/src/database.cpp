#include "../include/database.h"

namespace Core
{
	ChatDB::ChatDB() noexcept
	{
		mysql_init(&mysql);
		connection = mysql_real_connect(&mysql, "localhost", "root", "password", "hnet", 3306, NULL, 0);

		if (connection == NULL)
		{
			std::cerr << mysql_error(&mysql) << std::endl;
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
			std::cerr << mysql_error(&mysql) << std::endl;
			mysql_free_result(res);
			mysql_close(connection);
		}
	}

	bool ChatDB::InsertUser(TCPChat::Client::user_info* uinfo, struct sockaddr_in client_info)
	{
		std::string query_string = "INSERT INTO chat_user (username, login, password, ip, last_visited_at) VALUES (?, ?, ?, ?, NOW())";

		MYSQL_STMT* stmt = mysql_stmt_init(&mysql);

		if (!stmt)
		{
			std::cerr << "mysql_stmt_init(), out of memory\n";
			return false;
		}

		if (mysql_stmt_prepare(stmt, query_string.c_str(), query_string.length()) != 0)
		{
			std::cerr << "mysql_stmt_prepare(), INSERT failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
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

		if (mysql_stmt_bind_param(stmt, bind) != 0)
		{
			std::cerr << "mysql_stmt_bind_param() failed\n";
			mysql_stmt_close(stmt);
			return false;
		}

		if (mysql_stmt_execute(stmt) != 0)
		{
			std::cerr << "mysql_stmt_execute(), INSERT failed\n";
			mysql_stmt_close(stmt);
			return false;
		}

		mysql_stmt_close(stmt);

		return true;
	}

	bool ChatDB::UpdateUserInfo(const std::string& login)
	{
		std::string query_string = "UPDATE chat_user SET last_visited_at = NOW() WHERE login = '" + login + "'";

		if (mysql_query(&mysql, query_string.c_str()) != 0)
		{
			std::cerr << "mysql_query(), UPDATE failed for query: " << query_string << '\n';
			return false;
		}

		return true;
	}

	bool ChatDB::GetUsers(const std::string& username, TCPChat::Client::user_info_dto& data)
	{
		std::string query_string = "SELECT username FROM chat_user WHERE NOT username = ?";

		MYSQL_STMT* stmt = mysql_stmt_init(&mysql);
		if (!stmt)
		{
			std::cerr << "mysql_stmt_init(), out of memory\n";
			return false;
		}

		if (mysql_stmt_prepare(stmt, query_string.c_str(), query_string.length()) != 0)
		{
			std::cerr << "mysql_stmt_prepare(), GET failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
			return false;
		}

		MYSQL_BIND bind[1];
		memset(bind, 0, sizeof(bind));

		bind[0].buffer_type = MYSQL_TYPE_STRING;
		bind[0].buffer = const_cast<char*>(username.c_str());
		bind[0].buffer_length = username.length();
		bind[0].is_null = 0;

		if (mysql_stmt_bind_param(stmt, bind) != 0)
		{
			std::cerr << "mysql_stmt_bind_param() failed\n";
			mysql_stmt_close(stmt);
			return false;
		}

		if (mysql_stmt_execute(stmt) != 0)
		{
			std::cerr << "mysql_stmt_exectute(), GET failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
			return false;
		}

		std::vector<std::string> usernames;
		std::vector<char> result_data(256);
		unsigned long length;

		MYSQL_BIND result_bind;
		memset(&result_bind, 0, sizeof(result_bind));

		result_bind.buffer_type = MYSQL_TYPE_STRING;
		result_bind.buffer = result_data.data();
		result_bind.buffer_length = result_data.size();
		result_bind.length = &length;
		result_bind.is_null = 0;

		if (mysql_stmt_bind_result(stmt, &result_bind) != 0)
		{
			std::cerr << "mysql_stmt_bind_result() failed\n";
			mysql_stmt_close(stmt);
			return false;
		}

		while (mysql_stmt_fetch(stmt) == 0)
		{
			usernames.push_back(std::string(result_data.data(), length));
		}

		data.usernames = usernames;

		mysql_stmt_close(stmt);

		return true;
	}

	int ChatDB::GetIDByUsername(const std::string& username)
	{
		int user_id = 0;

		std::string query_string = "SELECT id FROM chat_user WHERE username = ?";

		MYSQL_STMT* stmt = mysql_stmt_init(&mysql);
		if (!stmt)
		{
			std::cerr << "mysql_stmt_init(), out of memory\n";
			return 0;
		}

		if (mysql_stmt_prepare(stmt, query_string.c_str(), query_string.length()) != 0)
		{
			std::cerr << "mysql_stmt_prepare(), GET failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
			return 0;
		}

		MYSQL_BIND bind[1];
		memset(bind, 0, sizeof(bind));

		bind[0].buffer_type = MYSQL_TYPE_STRING;
		bind[0].buffer = const_cast<char*>(username.c_str());
		bind[0].buffer_length = username.length();
		bind[0].is_null = 0;

		if (mysql_stmt_bind_param(stmt, bind) != 0)
		{
			std::cerr << "mysql_stmt_bind_param() failed\n";
			mysql_stmt_close(stmt);
			return 0;
		}

		MYSQL_BIND result_bind;
		memset(&result_bind, 0, sizeof(result_bind));

		result_bind.buffer_type = MYSQL_TYPE_LONG;
		result_bind.buffer = &user_id;
		result_bind.is_null = 0;

		if (mysql_stmt_bind_result(stmt, &result_bind) != 0)
		{
			std::cerr << "mysql_stmt_bind_result() failed\n";
			mysql_stmt_close(stmt);
			return 0;
		}

		if (mysql_stmt_execute(stmt) != 0)
		{
			std::cerr << "mysql_stmt_exectute(), GET failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
			return 0;
		}

		if (mysql_stmt_fetch(stmt) != 0)
		{
			std::cerr << "mysql_stmt_fetch(), GET failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
			return 0;
		}

		mysql_stmt_close(stmt);

		return user_id;
	}

	int ChatDB::GetIDByLogin(const std::string& login)
	{
		int user_id = 0;

		std::string query_string = "SELECT id FROM chat_user WHERE login = ?";

		MYSQL_STMT* stmt = mysql_stmt_init(&mysql);
		if (!stmt)
		{
			std::cerr << "mysql_stmt_init(), out of memory\n";
			return 0;
		}

		if (mysql_stmt_prepare(stmt, query_string.c_str(), query_string.length()) != 0)
		{
			std::cerr << "mysql_stmt_prepare(), GET failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
			return 0;
		}

		MYSQL_BIND bind[1];
		memset(bind, 0, sizeof(bind));

		bind[0].buffer_type = MYSQL_TYPE_STRING;
		bind[0].buffer = const_cast<char*>(login.c_str());
		bind[0].buffer_length = login.length();
		bind[0].is_null = 0;

		if (mysql_stmt_bind_param(stmt, bind) != 0)
		{
			std::cerr << "mysql_stmt_bind_param() failed\n";
			mysql_stmt_close(stmt);
			return 0;
		}

		MYSQL_BIND result_bind;
		memset(&result_bind, 0, sizeof(result_bind));

		result_bind.buffer_type = MYSQL_TYPE_LONG;
		result_bind.buffer = &user_id;
		result_bind.is_null = 0;

		if (mysql_stmt_bind_result(stmt, &result_bind) != 0)
		{
			std::cerr << "mysql_stmt_bind_result() failed\n";
			mysql_stmt_close(stmt);
			return 0;
		}

		if (mysql_stmt_execute(stmt) != 0)
		{
			std::cerr << "mysql_stmt_exectute(), GET failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
			return 0;
		}

		if (mysql_stmt_fetch(stmt) != 0)
		{
			std::cerr << "mysql_stmt_fetch(), GET failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
			return 0;
		}

		mysql_stmt_close(stmt);

		return user_id;
	}

	bool ChatDB::AddMessage(const std::string& message, const std::string& from, const std::string& to)
	{
		int user_id_from = GetIDByLogin(from);
		int user_id_to = GetIDByUsername(to);

		std::string query_string = "INSERT INTO chat_line (sender_id, receiver_id, message_text) VALUES (?, ?, ?)";

		MYSQL_STMT* stmt = mysql_stmt_init(&mysql);
		if (!stmt)
		{
			std::cerr << "mysql_stmt_init(), out of memory\n";
			return false;
		}

		if (mysql_stmt_prepare(stmt, query_string.c_str(), query_string.length()) != 0)
		{
			std::cerr << "mysql_stmt_prepare(), INSERT failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
			return false;
		}

		MYSQL_BIND bind[3];
		memset(bind, 0, sizeof(bind));

		bind[0].buffer_type = MYSQL_TYPE_LONG;
		bind[0].buffer = &user_id_from;
		bind[0].is_null = 0;

		bind[1].buffer_type = MYSQL_TYPE_LONG;
		bind[1].buffer = &user_id_to;
		bind[1].is_null = 0;

		bind[2].buffer_type = MYSQL_TYPE_STRING;
		bind[2].buffer = const_cast<char*>(message.c_str());
		bind[2].buffer_length = message.length();
		bind[2].is_null = 0;

		if (mysql_stmt_bind_param(stmt, bind) != 0)
		{
			std::cerr << "mysql_stmt_bind_param() failed\n";
			mysql_stmt_close(stmt);
			return false;
		}

		if (mysql_stmt_execute(stmt) != 0)
		{
			std::cerr << "mysql_stmt_execute(), INSERT failed for query: " << query_string << '\n';
			mysql_stmt_close(stmt);
			return false;
		}

		mysql_stmt_close(stmt);

		return true;
	}
}