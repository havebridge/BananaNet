#include "../include/server.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	UDPChat::Server server("127.0.0.1", 8888);

	if (server.Init())
	{
		server.Start();
	}



	return 0;
}