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
		std::cout << "CLIENT HANDER FUNC\n";
		struct sockaddr_in client_info;
		int client_info_lenght = sizeof(client_info);
		Client::user_info uinfo = {};
		Client::user_info_dto uinfo_dto = {};

		SOCKET client_socket = accept(server_socket, (sockaddr*)&client_info, &client_info_lenght);
		std::cout << "Client Socket:" << client_socket << '\n';
		if (client_socket == INVALID_SOCKET)
		{
			HN_ERROR("ClientHandler(): accept");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return;
		}

		client_count++;
		std::string recieved_buffer;
		int recieved_buffer_size = 0;

		if (recv(client_socket, (char*)&recieved_buffer_size, sizeof(int), 0) <= 0)
		{
			HN_ERROR("ClientHandler(): recieved_buffer_size recv");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
		}

		recieved_buffer.resize(recieved_buffer_size);

		if (recv(client_socket, recieved_buffer.data(), recieved_buffer_size, 0) <= 0)
		{
			HN_ERROR("ClientHandler(): recieved_buffer_size recv");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
		}

		json json_data = json::parse(recieved_buffer);

		uinfo.username = json_data["username"];
		uinfo.login = json_data["login"];
		uinfo.password = json_data["password"];
		uinfo.type = static_cast<Client::ConnectionType>(json_data["type"].get<int>());


		HN_INFO("username: {0} login: {1} password: {2} type: {3}", uinfo.username, uinfo.login, uinfo.password, uinfo.type);


		switch (uinfo.type)
		{
		case Client::ConnectionType::SIGN_UP:
		{
			//TODO: check if username or login already has in db, if so send to client appropriate message
			std::unique_ptr<Client> client(new Client(client_info, client_socket, uinfo));
			client_mutex.lock();
			HN_INFO("Client connected IP: {0} PORT: {1}", client->GetHost(), client->GetPort());
			db.InsertUser(&uinfo, client_info);
			clients.emplace_back(std::move(client));
			uinfo_dto.client_count = client_count - 1;
			db.GetUsers(uinfo.login, uinfo_dto);
			SendClientsInfo(uinfo_dto, client_socket);
			client_mutex.unlock();
		} break;
		case Client::ConnectionType::SIGN_IN:
		{
			if (SearchForClient(&uinfo))
			{
				HN_INFO("Client is found");
				client_mutex.lock();
				std::cout << "Client Socket:" << client_socket << '\n';
				UpdateSocket(client_socket, uinfo.login);
				uinfo_dto.client_count = client_count - 1;

				db.UpdateUserInfo(uinfo.login);
				db.GetUsers(uinfo.login, uinfo_dto);
				SendClientsInfo(uinfo_dto, client_socket);


				client_mutex.unlock();

				//TODO(): get client count with id in db
				//db.LoadMessageHistory();
			}
			else
			{
				HN_INFO("Client is not found");
			}
		} break;
		}

		GetClientsInfo();


		if (running)
		{
			thread_pool.AddJob(std::bind(&Server::ClientHandler, this));
		}

		if (clients.size() > 1)
		{
			thread_pool.AddJob(std::bind(&Server::ProcessData, this));
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
			if ((client->uinfo.login == uinfo->login) && (client->uinfo.password == uinfo->password))
			{
				return true;
			}
		}

		return false;
	}

	void Server::UpdateSocket(SOCKET new_socket, const std::string login)
	{
		auto it = std::find_if(clients.begin(), clients.end(), [&](const std::unique_ptr<Client>& client) {
			return client->uinfo.login == login;
			});

		(*it)->client_socket = new_socket;
	}

	bool Server::SendClientsInfo(const Client::user_info_dto& uinfo, SOCKET client_socket)
	{
		json json_data;
		json_data["usernames"] = uinfo.usernames;
		json_data["client_count"] = uinfo.client_count;
		std::string serialized_data = json_data.dump();
		int serialized_data_size = serialized_data.size();
		bool is_any_client_connected = uinfo.client_count == 0 ? false : true;
		std::cout << "IS any client connected = " << is_any_client_connected;

		if (send(client_socket, (const char*)&is_any_client_connected, sizeof(bool), 0) <= 0)
		{
			HN_ERROR("SendClientsInfo(): send is_any_client_connected");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		if (send(client_socket, (const char*)&serialized_data_size, sizeof(int), 0) <= 0)
		{
			HN_ERROR("SendClientsInfo(): send serialized_data");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		if (send(client_socket, serialized_data.c_str(), serialized_data_size, 0) <= 0)
		{
			HN_ERROR("SendClientsInfo(): send serialized_data");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool Server::SendMessageToClient(json json_data)
	{
		std::string username = json_data["to"];
		std::string message = json_data["message"];
		int message_size = message.size();


		auto client = std::find_if(clients.cbegin(), clients.cend(), [&](const std::unique_ptr<Client>& client) {
			return client->uinfo.username == username;
			});

		if (client != clients.cend())
		{
			if (send(client->get()->client_socket, (char*)static_cast<int>(message.size()), sizeof(int), 0) <= 0)
			{
				HN_ERROR("SendMessageToClient(): send message_size");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				return false;
			}

			if (send(client->get()->client_socket, message.c_str(), static_cast<int>(message.size()), 0) <= 0)
			{
				HN_ERROR("SendMessageToClient(): send message");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				return false;
			}

			return true;
		}
		else
		{
			HN_ERROR("SendMessageToClient(): find_if");
			return false;
		}
	}

	void Server::ProcessData()
	{
		std::cout << "PROCESS DATA FUNC\n";

		{
			std::lock_guard lock(client_mutex);
			for (auto it = clients.begin(), end = clients.end(); it != end; ++it)
			{
				auto& client = *it;
				if (client)
				{
					std::this_thread::sleep_for(std::chrono::microseconds(500));
					std::cout << "CONST AUTO& CLIENT : CLIENTS\n";
					Client::message_info message = {};
					SOCKET client_socket = client->client_socket;


					std::string recieved_buffer;
					int recieved_buffer_size = 0;
					unsigned long mode = 1;
					if (ioctlsocket(client_socket, FIONBIO, &mode) != 0)
					{
						//HN_ERROR("ProcessData(): ioctlsocket");
						//HN_ERROR("WSA Error: {0}", WSAGetLastError());
					}

					if (recv(client_socket, (char*)&recieved_buffer_size, sizeof(int), 0) <= 0)
					{
						//HN_ERROR("ClientHandler(): recieved_buffer_size recv");
						//HN_ERROR("WSA Error: {0}", WSAGetLastError());
						continue;
					}
					mode = 0;
					if (ioctlsocket(client_socket, FIONBIO, &mode) != 0)
					{
						//HN_ERROR("ProcessData(): ioctlsocket");
						//HN_ERROR("WSA Error: {0}", WSAGetLastError());
					}

					recieved_buffer.resize(recieved_buffer_size);

					if (recv(client_socket, recieved_buffer.data(), recieved_buffer_size, 0) <= 0)
					{
						//HN_ERROR("ClientHandler(): recieved_buffer_size recv");
						//HN_ERROR("WSA Error: {0}", WSAGetLastError());
					}


					json json_data = json::parse(recieved_buffer);

					message.message = json_data["message"];
					message.from = json_data["from"];
					message.to = json_data["to"];


					HN_INFO("message: {0} from: {1} to: {2}", message.message, message.from, message.to);

					db.AddMessage(message.message, message.from, message.to);

					SendMessageToClient(json_data);
				}

			}
		}
		if (running)
		{
			thread_pool.AddJob(std::bind(&Server::ProcessData, this));
		}
	}

	void Server::Start()
	{
		Init();
	}


	void Server::Stop()
	{
		thread_pool.Join();
		db.DeleteUsersInfo();
		//shutdown(server_socket, 0x02);
		closesocket(server_socket);
	}

	Server::~Server()
	{
		closesocket(server_socket);
		WSACleanup();
	}
}