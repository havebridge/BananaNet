#include "../include/server.h"
#include "../include/logger.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;


	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	//Core::Log::Init();
	//HN_CORE_WARN("Initialized Log!");
	//int a = 5;
	//("Hello var = {0}", a);

	UDPChat::Server server("127.0.0.1", 8888);

	if (server.Init())
	{
		server.Start();
	}

	server.Stop();

	return 0;
}