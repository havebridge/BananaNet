#include "../include/server.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	Hnet::Server server("127.0.0.1", 8888);

	server.Start();


	return 0;
}