#include "server.h"

#pragma warning(disable: 4996)

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

namespace UDPChat
{
	Server::Server(std::string ip, int port) noexcept
		:
		wsa{ 0 },
		server_socket(INVALID_SOCKET),
		ip(ip),
		port(port),
		server_info{ 0 },
		server_info_lenght(sizeof(server_info)),
		recieved_message(0),
		first_client_info{ 0 },
		second_client_info{ 0 },
		is_first_client_connected(false),
		is_second_client_connected(false) {}


	bool Server::Init()
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("WSAStartup");
			return false;
		}

		if ((server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("socket");
			return false;
		}

		unsigned long mode = 0;

		if (ioctlsocket(server_socket, FIONBIO, &mode) == SOCKET_ERROR)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("ioctlsocket");
			return false;
		}

		char yes = '1';

		if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == SOCKET_ERROR)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("setsockport");
			return false;
		}

		server_info.sin_family = AF_INET;
		server_info.sin_port = htons(port);
		server_info.sin_addr.s_addr = inet_addr(ip.c_str());

		ZeroMemory(server_info.sin_zero, 8);

		if (bind(server_socket, (const sockaddr*)&server_info, server_info_lenght) == SOCKET_ERROR)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("bind");
			return false;
		}

		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(server_socket, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);


		client_handler_file.open("../../../handler/file_handler.txt");

		if (client_handler_file.is_open())
		{
			client_handler_file << "1";
			client_handler_file.close();
		}
		else
		{
			std::cerr << "unable to open the file\n";
		}

		std::cout << "UDP Server started at:" << inet_ntoa(server_info.sin_addr) << ":" << htons(server_info.sin_port) << '\n';

		return true;
	}

	bool Server::ProcessFile(Instance::type client_handler)
	{
		client_handler_file.open("../../../handler/file_handler.txt");

		if (client_handler_file.is_open())
		{
			client_handler_file << static_cast<int>(client_handler);
			client_handler_file.close();
			return true;
		}
		else
		{
			std::cerr << "Unable to open the file\n";
			return false;
		}
	}

	void Server::ClientsHandler()
	{
		if (is_first_client_connected == false)
		{
			if (recvfrom(server_socket, (char*)(&is_first_client_connected), sizeof(bool), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom first client is_cliet_stared");
				return;
			}

			char get_info;

			if (recvfrom(server_socket, &get_info, sizeof(char), 0, (sockaddr*)&first_client_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom int");
				return;
			}

			is_first_client_connected = true;

			if (is_first_client_connected)
			{
				std::cout << "First client is handled\n";
				std::cout << "IP: " << inet_ntoa(first_client_info.sin_addr) << " PORT: " << htons(first_client_info.sin_port) << '\n';

				Instance::client_handler = Instance::type::first_client_handler;
				ProcessFile(Instance::client_handler);
			}
			else
			{
				std::cout << "The first client is not connected\n";
				return;
			}
		}
		else
		{
			if (recvfrom(server_socket, (char*)&is_second_client_connected, sizeof(bool), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom second client is_cliet_stared");
				return;
			}

			char get_info;

			if (recvfrom(server_socket, &get_info, sizeof(char), 0, (sockaddr*)&second_client_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom int");
				return;
			}

			is_second_client_connected = true;

			if (is_second_client_connected)
			{
				std::cout << "Second client is handled\n";
				std::cout << "IP: " << inet_ntoa(second_client_info.sin_addr) << " PORT: " << htons(second_client_info.sin_port) << '\n';

				Instance::client_handler = Instance::type::second_client_handler;
				ProcessFile(Instance::client_handler);
			}
			else
			{
				std::cout << "The second client is not conneted\n";
				return;
			}
		}
	}


	void Server::ProcessMessage()
	{
		if (recvfrom(server_socket, (char*)&recieved_message_size, sizeof(int), 0, (sockaddr*)&in, &server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recvfrom message size");
			return;
		}

		recieved_message = new char[recieved_message_size + 1];
		recieved_message[recieved_message_size] = '\0';

		if (recvfrom(server_socket, recieved_message, recieved_message_size, 0, (sockaddr*)&in, &server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recvfrom message");
			return;
		}

		int client = 0;

		if (recvfrom(server_socket, (char*)&client, sizeof(int), 0, (sockaddr*)&in, &server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("recvfrom message");
			return;
		}


		printf("Received packet from %s:%d\n", inet_ntoa(in.sin_addr), htons(in.sin_port));
		printf("Data: %s\n", recieved_message);

		send_message.assign(recieved_message, recieved_message_size);
		send_message_size = send_message.size();

		switch (static_cast<Instance::type>(client))
		{
		case Instance::type::first_client_handler:
		{
			std::cout << "First client handler\n";

			if (sendto(server_socket, (char*)&send_message_size, sizeof(int), 0, (const sockaddr*)&first_client_info, sizeof(first_client_info)) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("sendto second client message size");
				return;
			}

			if (sendto(server_socket, send_message.c_str(), send_message_size, 0, (const sockaddr*)&first_client_info, sizeof(first_client_info)) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("sendto second client message");
				return;
			}

			std::cout << "Send to " << inet_ntoa(first_client_info.sin_addr) << ':' << htons(first_client_info.sin_port) << '\n';
		} break;

		case Instance::type::second_client_handler:
		{
			std::cout << "Second client handler\n";

			if (sendto(server_socket, (char*)&send_message_size, sizeof(int), 0, (const sockaddr*)&second_client_info, sizeof(second_client_info)) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("sendto first client message size");
				return;
			}

			if (sendto(server_socket, send_message.c_str(), send_message_size, 0, (const sockaddr*)&second_client_info, sizeof(second_client_info)) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("sendto first client message");
				return;
			}

			std::cout << "Send to " << inet_ntoa(second_client_info.sin_addr) << ':' << htons(second_client_info.sin_port) << '\n';
		} break;

		default:
			std::cout << "Default\n";
			break;
		}

#if DEBUG
		std::cout << "message size send: " << recieved_message_size;
		std::cout << "\nmessage send: " << recieved_message;
		std::cout << '\n';
#endif

		delete[] recieved_message;
	}

	void Server::Start()
	{
		while (is_first_client_connected == false || is_second_client_connected == false)
		{
			ClientsHandler();
		}

		while (is_first_client_connected && is_second_client_connected)
		{
			ProcessMessage();
		}
	}


	void Server::Stop()
	{
		closesocket(server_socket);
	}

	Server::~Server()
	{
		WSACleanup();
		closesocket(server_socket);
	}
}