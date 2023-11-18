#include "../include/database.h"

namespace Core
{
	ChatDB::ChatDB()
	{
		connect = mysql_real_connect(connect, "localhost", "root", "password", "testdb", 3306, NULL, 0);
		if (connect)
		{
			std::string query = "SELECT * FROM test";
			const char* q = query.c_str();
			qstate = mysql_query(connect, q);

			if (!qstate)
			{
				res = mysql_store_result(connect);
				while (row = mysql_fetch_row(res))
				{
					std::cout << "ID: " << row[0] << " Name: " << row[1] << " Value: " << row[2] << '\n';
				}
			}
		}
		else
		{
			std::cout << mysql_error(connect) << std::endl;
		}
	}
}