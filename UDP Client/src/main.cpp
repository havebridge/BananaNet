#include "../include/client.h"

static void usage(void)
{
	std::cout << "Usage: udp_client client_ip server_ip\n";
}

int main(int argc, char** argv)
{
	/*if (argc != 3)
	{
		usage();
		return 0;
	}*/

	UDPChat::Client client("127.0.0.1");

	if (client.Connect("127.0.0.1", 8888))
	{
		client.Run();
	}

	return 0;
}