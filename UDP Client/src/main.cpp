#include "../include/client.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	UDPChat::Client client;

	if (client.Connect("127.0.0.1", 8888))
	{
		client.Run();
	}

	return 0;
}