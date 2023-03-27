#include "../include/server.h"


namespace Hnet
{
	Server::Server(std::string ip, int port)
		:
		wsa{ 0 },
		serversocket(INVALID_SOCKET),
		ip(ip),
		port(port),
		info{ 0 },
		infolength(sizeof(info)),
		recvlength(0),
		sendlength(0) {}


	void Server::Start()
	{
		setup();

		for (;;)
		{

		}
	}


	Server::~Server()
	{
		WSACleanup();
		closesocket(serversocket);
	}
}