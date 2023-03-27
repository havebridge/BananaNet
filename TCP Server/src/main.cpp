#include "../include/server.h"

int main(int argc, char** argv)
{
#ifndef RELEASE
	if (argc != 2)
	{
		fprintf(stderr, "usage: server.exe hostname");
		return 1;
	}


	Hnet::Server server("127.0.0.1", 8888);
	server.Start();
#endif

#ifdef DEBUG
	Hnet::Server server("127.0.0.1", 8888);
	server.Start();
#endif


	std::cout << "Hey\n";
	return 0;
}