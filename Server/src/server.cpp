#include "../include/server.h"

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
			HN_ERROR("WSAStartup() failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		if ((server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
		{
			HN_ERROR("socket() failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		unsigned long mode = 0;

		if (ioctlsocket(server_socket, FIONBIO, &mode) == SOCKET_ERROR)
		{
			HN_ERROR("ioctlsocket() failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		char yes = '1';

		if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == SOCKET_ERROR)
		{
			HN_ERROR("setsockopt() failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		server_info.sin_family = AF_INET;
		server_info.sin_port = htons(port);
		server_info.sin_addr.s_addr = inet_addr(ip.c_str());

		ZeroMemory(server_info.sin_zero, 8);

		if (bind(server_socket, (const sockaddr*)&server_info, server_info_lenght) == SOCKET_ERROR)
		{
			HN_ERROR("bind() failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(server_socket, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);

		Core::Log::Init();

		client_handler_file.open("../../../../handler/file_handler.txt");

		if (client_handler_file.is_open())
		{
			client_handler_file << "0";
			client_handler_file.close();
		}
		else
		{
			HN_ERROR("Unable to open the file");
		}

		HN_INFO("UDP Server started at: {0}:{1}", inet_ntoa(server_info.sin_addr), htons(server_info.sin_port));

		return true;
	}

	bool Server::ProcessFile(Instance::type client_handler)
	{
		client_handler_file.open("../../../../handler/file_handler.txt");

		if (client_handler_file.is_open())
		{
			client_handler_file << static_cast<int>(client_handler);
			client_handler_file.close();
			return true;
		}
		else
		{
			HN_ERROR("Unable to open the file");
			return false;
		}
	}

	void Server::ClientsHandler()
	{
		if (is_first_client_connected == false)
		{
			if (recvfrom(server_socket, (char*)(&is_first_client_connected), sizeof(bool), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				HN_ERROR("recvfom(is_first_client_connected) is failed");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				return;
			}

			char get_info;

			if (recvfrom(server_socket, &get_info, sizeof(char), 0, (sockaddr*)&first_client_info, &server_info_lenght) <= 0)
			{
				HN_ERROR("recvfom(get_info) is failed");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				return;
			}

			is_first_client_connected = get_info;

			if (is_first_client_connected)
			{
				HN_INFO("First client is handled");
				HN_INFO("IP: {0} PORT: {1}", inet_ntoa(first_client_info.sin_addr), htons(first_client_info.sin_port));

				Instance::client_handler = Instance::type::second_client_handler;
				ProcessFile(Instance::client_handler);
			}
			else
			{
				HN_ERROR("The first client is not connected");
				return;
			}
		}
		else
		{
			if (recvfrom(server_socket, (char*)&is_second_client_connected, sizeof(bool), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				HN_ERROR("recvfom(is_second_client_connected) is failed");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				return;
			}

			char get_info;

			if (recvfrom(server_socket, &get_info, sizeof(char), 0, (sockaddr*)&second_client_info, &server_info_lenght) <= 0)
			{
				HN_ERROR("recvfom(get_info) is failed");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				return;
			}

			is_second_client_connected = get_info;

			if (is_second_client_connected)
			{
				HN_INFO("Second client is handled");
				HN_INFO("IP: {0} PORT: {1}", inet_ntoa(second_client_info.sin_addr), htons(second_client_info.sin_port));

				Instance::client_handler = Instance::type::first_client_handler;
				ProcessFile(Instance::client_handler);
			}
			else
			{
				HN_ERROR("The second client is not connected");
				return;
			}
		}
	}


	void Server::ProcessMessage()
	{
		if (recvfrom(server_socket, (char*)&recieved_message_size, sizeof(int), 0, (sockaddr*)&in, &server_info_lenght) <= 0)
		{
			HN_ERROR("recvfom(recieved_message_size) is failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return;
		}

		recieved_message = new char[recieved_message_size + 1];
		recieved_message[recieved_message_size] = '\0';

		if (recvfrom(server_socket, recieved_message, recieved_message_size, 0, (sockaddr*)&in, &server_info_lenght) <= 0)
		{
			HN_ERROR("recvfom(recieved_message) is failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return;
		}

		int client = 0;

		if (recvfrom(server_socket, (char*)&client, sizeof(int), 0, (sockaddr*)&in, &server_info_lenght) <= 0)
		{
			HN_ERROR("recvfom(client) is failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return;
		}


		HN_INFO("Received packet from {0}:{1}", inet_ntoa(in.sin_addr), htons(in.sin_port));
		HN_INFO("Data: {0}", recieved_message);

		send_message.assign(recieved_message, recieved_message_size);
		send_message_size = send_message.size();

		switch (static_cast<Instance::type>(client))
		{
		case Instance::type::first_client_handler:
		{
			HN_WARN("First client handler");

			if (sendto(server_socket, (char*)&send_message_size, sizeof(int), 0, (const sockaddr*)&second_client_info, sizeof(second_client_info)) <= 0)
			{
				HN_ERROR("sendto(send_message_size) to second client is failed");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				return;
			}

			if (sendto(server_socket, send_message.c_str(), send_message_size, 0, (const sockaddr*)&second_client_info, sizeof(second_client_info)) <= 0)
			{
				HN_ERROR("sendto(send_message) to second client is failed");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				return;
			}

			HN_INFO("Send to {0}:{1}", inet_ntoa(second_client_info.sin_addr), htons(second_client_info.sin_port));
		} break;

		case Instance::type::second_client_handler:
		{
			HN_WARN("Second client handler");

			if (sendto(server_socket, (char*)&send_message_size, sizeof(int), 0, (const sockaddr*)&first_client_info, sizeof(first_client_info)) <= 0)
			{
				HN_ERROR("sendto(send_message_size) to first client is failed");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				return;
			}

			if (sendto(server_socket, send_message.c_str(), send_message_size, 0, (const sockaddr*)&first_client_info, sizeof(first_client_info)) <= 0)
			{
				HN_ERROR("sendto(send_message) to first client is failed");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				return;
			}

			HN_INFO("Send to {0}:{1}", inet_ntoa(first_client_info.sin_addr), htons(first_client_info.sin_port));
		} break;

		default:
			HN_ERROR("Default");
			break;
		}

#if DEBUG
		HN_INFO("message size send: {0}", recieved_message_size);
		HN_INFO("message send: {0}", recieved_message);
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