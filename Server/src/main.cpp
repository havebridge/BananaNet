#include "../include/server.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	TCPChat::Server server("127.0.0.1", 8888);

	server.Start();

	server.Stop();	

	system("pause");
	return 0;
}