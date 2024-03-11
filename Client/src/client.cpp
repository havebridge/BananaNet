#include "../include/client.h"

#pragma warning(disable: 4996)

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

namespace TCPChat
{
	Client::Client() noexcept
		:
		wsa{ 0 },
		client_socket(INVALID_SOCKET),
		client_info{ 0 },
		client_info_lenght(sizeof(client_info)),
		server_info{ 0 },
		server_info_lenght(sizeof(server_info)),
		recieved_message_size(0) {}


	bool Client::Connect(const std::string& ip, int port)
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("WSAStartup");
			return false;
		}

		if ((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("socket");
			return false;
		}

		char mode = '1';

		if ((setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &mode, sizeof(char)) == SOCKET_ERROR))
		{
			std::cout << WSAGetLastError() << '\n';
			perror("setsockport");
			return false;
		}

		server_info.sin_family = PF_INET;
		server_info.sin_port = htons(port);
		server_info.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

		ZeroMemory(server_info.sin_zero, 8);

		if (connect(client_socket, (const sockaddr*)&server_info, server_info_lenght) == SOCKET_ERROR)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("client connect");
			return false;
		}

		if (getsockname(client_socket, (sockaddr*)&client_info, &client_info_lenght) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("getsockname");
			return false;
		}

		GetClientExternalIp();	//mb shoudl`t use

		std::cout << std::format("Client info\nExternal ip: {} port: {}\n", client_external_ip, client_info.sin_port);

		is_connected = true;

		return is_connected;
	}

	void Client::GetClientExternalIp()
	{
		HINTERNET net = InternetOpenW(L"IP retriever", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		HINTERNET conn = InternetOpenUrlW(net, L"http://myexternalip.com/raw", NULL, 0, INTERNET_FLAG_RELOAD, 0);

		char buffer[4096];
		DWORD read;

		InternetReadFile(conn, buffer, sizeof(buffer) / sizeof(buffer[0]), &read);
		InternetCloseHandle(net);

		client_external_ip = std::string(buffer, read);
	}

	bool Client::SendUserInfoSignUp(const std::string& username, const std::string& login, const std::string& password)
	{
		json jsonData = {
			{"username", username},
			{"login", login},
			{"password", password},
		};

		std::string serialized_data = jsonData.dump();
		int serialized_data_size = serialized_data.size();

		if (send(client_socket, reinterpret_cast<const char*>(&serialized_data_size), sizeof(int), 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("SendUserInfoSignUp(): serialized_data_size send");
			return false;
		}

		if (send(client_socket, serialized_data.c_str(), serialized_data_size, 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("SendUserInfoSignUp(): serialized_data send");
			return false;
		}

		bool is_exist = false;

		if (recv(client_socket, (char*)&is_exist, sizeof(bool), 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("SendUserInfoSignUp(): is_exist recv");
			return false;
		}

		return (is_exist == true) ? false : true;
	}

	bool Client::SendUserInfoSignIn(const std::string& login, const std::string& password)
	{
		json json_data = {
			{"username", std::string()},
			{"login", login},
			{"password", password},
		};

		std::string serialized_data = json_data.dump();
		int serialized_data_size = serialized_data.size();

		if (send(client_socket, reinterpret_cast<const char*>(&serialized_data_size), sizeof(int), 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("SendUserInfoSignUp(): serialized_data_size send");
			return false;
		}

		if (send(client_socket, serialized_data.c_str(), serialized_data_size, 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("SendUserInfoSignUp(): serialized_data send");
			return false;
		}

		bool is_found = false;

		if (recv(client_socket, (char*)&is_found, sizeof(bool), 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("SendUserInfoSignUp(): is_found recv");
			return false;
		}

		return is_found;
	}


	void Client::SendButtonInfo(int button_type)
	{
		if (send(client_socket, reinterpret_cast<const char*>(&button_type), sizeof(int), 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("SendButtonInfo(): type button send");
		}
	}

	void Client::RecieveUsersInfo()
	{
		std::string recieved_buffer;
		int recieved_buffer_size = 0;
		bool is_any_client_connected = false;

		if (recv(client_socket, (char*)&is_any_client_connected, sizeof(bool), 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("RecieveUsersInfo(): is_any_client_connected recv");
		}

		std::cout << "IS any client connected = " << is_any_client_connected;

		if (is_any_client_connected == true)
		{
			if (recv(client_socket, (char*)&recieved_buffer_size, sizeof(int), 0) == -1)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("RecieveUsersInfo(): recieved_buffer_size recv");
			}

			recieved_buffer.resize(recieved_buffer_size);

			if (recv(client_socket, recieved_buffer.data(), recieved_buffer_size, 0) == -1)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("RecieveUsersInfo(): recieved_buffer_size recv");
			}

			json json_data = json::parse(recieved_buffer);

			uinfo_dto.usernames = json_data["usernames"];
			uinfo_dto.client_count = json_data["client_count"];
		}

		std::cout << "USERNAMES: ";
		for (const auto& username : uinfo_dto.usernames)
		{
			std::cout << username << '\n';
		}
		std::cout << "Client count: " << uinfo_dto.client_count << '\n';
	}

	void Client::SendMessageText(const std::string& message, const std::string& from, const std::string& to)
	{
		json json_data;
		json_data["message"] = message;
		json_data["from"] = from;
		json_data["to"] = to;
		std::string serialized_data = json_data.dump();
		int serialized_data_size = serialized_data.size();

		if (send(client_socket, reinterpret_cast<const char*>(&serialized_data_size), sizeof(int), 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("SendMessageTest(): recieved_buffer_size send");
		}

		if (send(client_socket, serialized_data.c_str(), serialized_data_size, 0) == -1)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("SendMessageTest(): recieved_buffer send");
		}


		std::cout << "Message Send: " << message << "\n\n";
	}

	void Client::RecieveMessageText()
	{
		std::string recieved_message;
		int recieved_message_size = 0;


		if (recv(client_socket, (char*)&recieved_message_size, sizeof(int), 0) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("RecieveUsersInfo(): recieved_buffer_size recv");
		}

		recieved_message.resize(recieved_message_size);

		if (recv(client_socket, recieved_message.data(), recieved_message_size, 0) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("RecieveUsersInfo(): recieved_buffer_size recv");
		}
	}

	void Client::Disconnect()
	{
		std::cout << "Disconnect\n";
		shutdown(client_socket, SD_SEND);
		closesocket(client_socket);
		is_connected = false;
	}


	void Client::Run()
	{
		while (true);

		Disconnect();
	}

	Client::~Client()
	{
		if (recv_thread.joinable())
		{
			recv_thread.join();
		}
		closesocket(client_socket);
		WSACleanup();
	}
}