#include "server.h"
#include "Log.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	Logger::Log::Init();
	Logger::Log::GetCoreLogger()->warn("Initialized Log!");
	Logger::Log::GetClientLogger()->info("Hello Server!");

	UDPChat::Server server("127.0.0.1", 8888);

	if (server.Init())
	{
		server.Start();
	}

	server.Stop();

	return 0;
}