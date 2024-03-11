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

		HN_INFO("TCP Server started at: {0}:{1}", inet_ntoa(server_info.sin_addr), htons(server_info.sin_port));

		running = true;

		return running;
	}

	void Server::SendInfoMessageToClient(SOCKET client_socket, bool is_exist)
	{
		if (send(client_socket, reinterpret_cast<const char*>(&is_exist), sizeof(bool), 0) == -1)
		{
			HN_ERROR("SendClientsInfo(): send is_any_client_connected");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
		}
	}

	void Server::GetUserInfo(SOCKET client_socket, Client::user_info& client_info)
	{
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

		client_info.username = json_data["username"];
		client_info.login = json_data["login"];
		client_info.password = json_data["password"];
	}

	bool Server::HandleSignUp(SOCKET client_socket, sockaddr_in client_info_sockaddr, Client::user_info& client_info, Client::user_info_dto& client_info_dto)
	{
		bool is_exist = db.UserExist(client_info.username, client_info.login);
		SendInfoMessageToClient(client_socket, is_exist);

		if (is_exist)
		{
			return false;
		}

		std::unique_ptr<Client> client(new Client(client_info_sockaddr, client_socket, client_info));

		client_mutex.lock();
		HN_INFO("Client connected IP: {0} PORT: {1}", client->GetHost(), client->GetPort());
		db.InsertUser(&client_info, client_info_sockaddr);
		clients.emplace_back(std::move(client));
		client_info_dto.client_count = client_count - 1;
		db.GetUsers(client_info.login, client_info_dto);
		SendClientsInfo(client_info_dto, client_socket);
		client_mutex.unlock();

		return true;
	}

	bool Server::HandleSignIn(SOCKET client_socket, Client::user_info client_info, Client::user_info_dto client_info_dto)
	{
		if (SearchForClient(&client_info))
		{
			HN_INFO("Client is found");
			bool is_found = true;
			if (send(client_socket, reinterpret_cast<const char*>(&is_found), sizeof(bool), 0) == -1)
			{
				HN_ERROR("ClientHandler(): is_found send");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
			}


			client_mutex.lock();
			std::cout << "Client Socket:" << client_socket << '\n';
			UpdateSocket(client_socket, client_info.login);
			client_info_dto.client_count = client_count - 1;

			db.UpdateUserInfo(client_info.login);
			db.GetUsers(client_info.login, client_info_dto);
			SendClientsInfo(client_info_dto, client_socket);


			client_mutex.unlock();
			return true;
		}
		else
		{
			HN_INFO("Client is not found");
			bool is_found = false;
			if (send(client_socket, reinterpret_cast<const char*>(&is_found), sizeof(bool), 0) == -1)
			{
				HN_ERROR("ClientHandler(): is_found send");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
			}

			return false;
		}
	}

	void Server::ClientHandler()
	{
		std::cout << "CLIENT HANDER FUNC\n";
		bool client_connected = false;

		struct sockaddr_in client_info_sockaddr;
		int client_info_lenght = sizeof(client_info_sockaddr);

		Client::user_info client_info = {};
		Client::user_info_dto client_info_dto = {};

		SOCKET client_socket = accept(server_socket, (sockaddr*)&client_info_sockaddr, &client_info_lenght);
		std::cout << "Client Socket:" << client_socket << '\n';
		if (client_socket == INVALID_SOCKET)
		{
			HN_ERROR("ClientHandler(): accept");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			thread_pool.AddJob(std::bind(&Server::ClientHandler, this));
			return;
		}

		client_count++;

		while (!client_connected)
		{
			Client::ButtonType button_type = static_cast<Client::ButtonType>(GetButtonType(client_socket));

			switch (button_type)
			{
			case Client::ButtonType::SIGN_UP:
			{
				HN_INFO("Client Pressed Sign up Button");
				GetUserInfo(client_socket, client_info);
				client_connected = HandleSignUp(client_socket, client_info_sockaddr, client_info, client_info_dto);
			} break;
			case Client::ButtonType::SIGN_IN:
			{
				HN_INFO("Client Pressed Sign In Button");
				GetUserInfo(client_socket, client_info);
				client_connected = HandleSignIn(client_socket, client_info, client_info_dto);
			} break;
			case Client::ButtonType::BACK:
			{
				HN_INFO("Client Pressed Back Button");
			} break;
			default:
			{
				HN_ERROR("Client is disconnected by uncommon incident");
				thread_pool.AddJob(std::bind(&Server::ClientHandler, this));
				return;
			} break;
			}
		}

		GetClientsInfo();

		HN_INFO("username: {0} login: {1} password: {2}", client_info.username, client_info.login, client_info.password);

		if (running)
		{
			thread_pool.AddJob(std::bind(&Server::ClientHandler, this));
		}

		if (clients.size() == 2)
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

	int Server::GetButtonType(SOCKET client_socket)
	{
		int button_type = 0;

		if (recv(client_socket, (char*)&button_type, sizeof(int), 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("GetButtonType(): button_type recv");
		}

		return button_type;
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

		if (send(client_socket, reinterpret_cast<const char*>(&is_any_client_connected), sizeof(bool), 0) == -1)
		{
			HN_ERROR("SendClientsInfo(): send is_any_client_connected");
			HN_ERROR("WSA Error: {0}", WSAGetLastError());
			return false;
		}

		if (send(client_socket, reinterpret_cast<const char*>(&serialized_data_size), sizeof(int), 0) == -1)
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


		auto client = std::find_if(clients.begin(), clients.end(), [&](const std::unique_ptr<Client>& client) {
			return client->uinfo.username == username;
			});

		if (client != clients.end())
		{
			int message_size_net_order = htonl(message_size);

			if (send(client->get()->client_socket, reinterpret_cast<char*>(&message_size_net_order), sizeof(int), 0) == -1)
			{
				HN_ERROR("SendMessageToClient(): send message_size");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				thread_pool.AddJob(std::bind(&Server::ProcessData, this));
				return false;
			}

			if (send(client->get()->client_socket, message.c_str(), static_cast<int>(message.size()), 0) == -1)
			{
				HN_ERROR("SendMessageToClient(): send message");
				HN_ERROR("WSA Error: {0}", WSAGetLastError());
				thread_pool.AddJob(std::bind(&Server::ProcessData, this));
				return false;
			}

			return true;
		}
		else
		{
			HN_ERROR("SendMessageToClient(): find_if");
			thread_pool.AddJob(std::bind(&Server::ProcessData, this));
			return false;
		}
	}

	void Server::ProcessData()
	{
		{
			std::lock_guard lock(client_mutex);
			for (auto it = clients.begin(), end = clients.end(); it != end; ++it)
			{
				auto& client = *it;
				if (client)
				{
					Client::message_info message = {};
					SOCKET client_socket = client->client_socket;


					std::string recieved_buffer;
					int recieved_buffer_size = 0;
					unsigned long mode = 1;
					ioctlsocket(client_socket, FIONBIO, &mode);


					if (recv(client_socket, (char*)&recieved_buffer_size, sizeof(int), 0) <= 0)
					{
						continue;
					}
					mode = 0;
					ioctlsocket(client_socket, FIONBIO, &mode);

					recieved_buffer.resize(recieved_buffer_size);

					recv(client_socket, recieved_buffer.data(), recieved_buffer_size, 0);

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
		closesocket(server_socket);
	}

	Server::~Server()
	{
		closesocket(server_socket);
		WSACleanup();
	}
}