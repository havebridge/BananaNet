#include "../include/client.h"


int main(int argc, char** argv)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	TCPChat::Client client;
	if (client.Connect("127.0.0.1", 8888))
	{
		client.Run();
	}

	system("pause");
	return 0;
}