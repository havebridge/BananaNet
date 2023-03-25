#include "../include/client.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	Hnet::Client client("127.0.0.1", 8888);

	client.Connect();

	return 0;
}