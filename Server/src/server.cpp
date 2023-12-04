#include "../include/server.h"

#pragma warning(disable: 4996)

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)


namespace TCPChat
{
	Server::Server(std::string ip, int port) noexcept
		:
		wsa{ 0 },
		server_socket(INVALID_SOCKET),
		ip(ip),
		port(port),
		server_info{ 0 },
		server_info_lenght(sizeof(server_info)),
		running(false) {}

	std::string Client::GetHost() const
	{
		return std::string(inet_ntoa(client_info.sin_addr));
	}

	std::string Client::GetPort() const
	{
		return std::to_string(client_info.sin_port);
	}

	bool Server::Init()
	{
		Core::Log::Init();

		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
		{
			HN_ERROR("WSAStartup() failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		if ((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
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

		if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
		{
			HN_ERROR("listen() failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		thread_pool.AddJob(std::bind(&Server::ClientHandler, this));
		//thread_pool.AddJob(std::bind(&Server::ProcessData, this));

#if 0
		BOOL bNewBehavior = FALSE;
		DWORD dwBytesReturned = 0;
		WSAIoctl(server_socket, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);
#endif

		HN_INFO("TCP Server started at: {0}:{1}", inet_ntoa(server_info.sin_addr), htons(server_info.sin_port));

		running = true;

		return running;
	}

	void Server::ClientHandler()
	{
		struct sockaddr_in client_info;
		int client_info_lenght = sizeof(client_info);
		Client::user_info uinfo = {};
		
		SOCKET client_socket = accept(server_socket, (sockaddr*)&client_info, &client_info_lenght);
		if (client_socket == INVALID_SOCKET)
		{
			HN_ERROR("accept() failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return;
		}

		char* recieved_buffer = new char[sizeof(Client::user_info)];
		if (recv(client_socket, recieved_buffer, sizeof(Client::user_info), 0) <= 0)
		{
			HN_ERROR("recv() user_info failed");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			delete[] recieved_buffer;
			return;
		}

		memcpy(&uinfo, recieved_buffer, sizeof(Client::user_info));

		switch (uinfo.type)
		{
		case Client::ConnectionType::SIGN_UP:
		{
			std::unique_ptr<Client> client(new Client(client_info, client_socket, uinfo));
			client_mutex.lock();
			HN_INFO("Client connected IP: {0} PORT: {1}", client->GetHost(), client->GetPort());
			db.InsertUserTo(&uinfo, client_info);
			clients.emplace_back(std::move(client));
			client_mutex.unlock();
		} break;
		case Client::ConnectionType::SIGN_IN:
		{
			if (SearchForClient(&uinfo))
			{
				HN_INFO("Client is found");
			}
			else
			{
				HN_INFO("Client is not found");
			}
		} break;
		}

		GetClientsInfo();

		delete[] recieved_buffer;

		if (running)
		{
			thread_pool.AddJob(std::bind(&Server::ClientHandler, this));
		}
	}

	void Server::GetClientsInfo()
	{
		HN_INFO("All Clients:");
		for (const auto& client : clients)
		{
			HN_INFO("Client info IP: {0} PORT: {1} USERNAME: {2} LOGIN: {3} PASSWORD: {4}", client->GetHost(), client->GetPort(), 
					client->uinfo.username, client->uinfo.login, client->uinfo.password);
		}
	}

	bool Server::SearchForClient(Client::user_info* uinfo)
	{
		for (const auto& client : clients)
		{
			if (strcmp(client->uinfo.login, uinfo->login) == 0)
			{
				return true;
			}
		}

		return false;
	}

	void Server::ProcessData()
	{
		{
			std::lock_guard<std::mutex> client_lock(client_mutex);
			for (auto it = clients.begin(), end = clients.end(); it != end; ++it)
			{
				auto& client = *it;
				if (client)
				{
					std::cout << "OK\n";
					//TODO(): process data
				}
				else if (client->is_connected == false)
				{
					client->client_handler.lock();
					HN_INFO("Client disconnected IP: {0} PORT: {1}", client->GetHost(), client->GetPort());
					client->client_handler.unlock();
					client.release();
					clients.erase(it);
				}
			}
		}
		if (running)
		{
			thread_pool.AddJob(std::bind(&Server::ProcessData, this));
		}
	}


#if 0
		while (running)
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
	}
#endif

#if 0
	void Server::ProcessMessage()
	{
		while (true)
		{
			std::unique_lock<std::mutex> message_lock(message_mutex);

			message_cv.wait(message_lock, [this] {
				return is_first_client_connected && is_second_client_connected;
				});

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
				HN_ERROR("There is no another connected client");
				break;
			}

#if DEBUG
			HN_INFO("message size send: {0}", recieved_message_size);
			HN_INFO("message send: {0}", recieved_message);
#endif
			delete[] recieved_message;
		}
	}
#endif

	void Server::Start()
	{
		Init();

		/*while (is_first_client_connected == false || is_second_client_connected == false)
		{
			ClientsHandler();
		}*/
		//message_thread = (std::thread(&Server::ProcessMessage, this));
		//while (true)
		//{
			//ProcessMessage();
		//}
	}


	void Server::Stop()
	{
		thread_pool.Join();
		closesocket(server_socket);
	}

	Server::~Server()
	{
		closesocket(server_socket);
		WSACleanup();
	}
}