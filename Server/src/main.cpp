#include "../include/server.h"
#include "../include/logger.h"

using namespace std::chrono_literals;

void test_task()
{
	std::cout << "[" << std::this_thread::get_id() << "]\n";
}

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	/*UDPChat::Server server("127.0.0.1", 8888);

	if (server.Init())
	{
		server.Start();
	}

	server.Stop();*/

	//test thread pool
	Core::ThreadPool thread_pool;

	for (int i = 0; i != 100; ++i)
	{
		thread_pool.AddJob(test_task);
		std::this_thread::sleep_for(1ms);
	}

	

	return 0;
}
