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

	/*bool ChatDB::InsertUserTo()
	{
		int a = 0;
		qstate = (mysql_query(&mysql, "INSERT into ''hnet'.'chat' ('id') VALUES ('1')'"));
		if (qstate != 0)
		{
			std::cout << mysql_error(&mysql) << std::endl;
			mysql_close(&mysql);
			return false;
		}

		return true;
	}*/
}