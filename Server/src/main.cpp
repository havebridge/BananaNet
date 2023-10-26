#include "../include/server.h"
#include "../include/logger.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	UDPChat::Server server("127.0.0.1", 8888);

	if (server.Init())
	{
		server.Start();
	}

	server.Stop();

	return 0;
}