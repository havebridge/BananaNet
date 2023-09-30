#include "../include/client.h"

#pragma warning(disable: 4996)


namespace UDPChat
{
	Client::Client()
		:
		wsa{ 0 },
		client_socket(INVALID_SOCKET),
		client_info{ 0 },
		client_info_lenght(sizeof(client_info)),
		is_connected(false),
		recv_message_size(0) {}


	bool Client::Connect(std::string ip, int port)
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("WSAStartup");
			return false;
		}

		if ((client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("socket");
			return false;
		}

		/*char yes = '1';

		if ((setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1))
		{
			std::cout << WSAGetLastError() << '\n';
			perror("setsockport");
			return false;
		}*/

		client_info.sin_family = AF_INET;
		client_info.sin_port = htons(port);
		client_info.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

		ZeroMemory(client_info.sin_zero, 8);

		return true;
	}

	bool Client::ProcessFile(const char* file_name)
	{
		client_handler_file.open(file_name);

		if (client_handler_file.is_open())
		{
			int res;
			client_handler_file >> res;

			client_type = static_cast<Instance::type>(res);

			std::cout << res;

			client_handler_file.close();
			return true;
		}
		else
		{
			std::cerr << "Unable to open the file\n";
			return false;
		}
	}

	bool Client::SendInfo()
	{
		if (sendto(client_socket, inet_ntoa(client_info.sin_addr), INET_ADDRSTRLEN, 0, (const sockaddr*)&client_info, client_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto first client ip");
			return false;
		}

		int port = client_info.sin_port;

		if (sendto(client_socket, (char*)&port, sizeof(int), 0, (const sockaddr*)&client_info, client_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto first client port");
			return false;
		}

		is_connected = true;

		ProcessFile("C:/Users/meylor/source/repos/hnet/handler/file_handler.txt");

		return is_connected;
	}

	void Client::SendMSG()
	{
		std::cout << "SendMSG\n";
		std::cout << "Send message: ";
		std::getline(std::cin, send_message);

		send_message_size = send_message.size();

		if (sendto(client_socket, (char*)&send_message_size, sizeof(int), 0, (const sockaddr*)&client_info, client_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto message size");
			return;
		}

		if (sendto(client_socket, send_message.c_str(), send_message_size, 0, (const sockaddr*)&client_info, client_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto message");
			return;
		}

		int client = static_cast<int>(client_type);

		if (sendto(client_socket, (char*)&client, sizeof(int), 0, (const sockaddr*)&client_info, client_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto message");
			return;
		}

		cv.notify_one();

		std::cout << "message size send: " << send_message_size;
		std::cout << "\nmessage send:" << send_message.data();
		std::cout << "\nclient type send:" << client;
		std::cout << '\n';
	}

	void Client::RecvMSG()
	{
		while (true) 
		{
			std::unique_lock<std::mutex> recv_lock(recv_mutex);

			cv.wait(recv_lock, [this]  {
				return !is_sended;
				});

			std::cout << "RecvMSG\n";

			if (recvfrom(client_socket, (char*)&recv_message_size, sizeof(int), 0, (sockaddr*)&client_info, &client_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom message size");
				return;
			}

			recv_message = new char[recv_message_size + 1];
			recv_message[recv_message_size] = '\0';

			if (recvfrom(client_socket, recv_message, recv_message_size, 0, (sockaddr*)&client_info, &client_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom message");
				return;
			}

			std::cout << "RECV MESSAGE\n";
			std::cout << "message size recv: " << recv_message_size;
			std::cout << "\nmessage recv: " << recv_message;
			std::cout << '\n';

			delete[] recv_message;
		}
	}


	void Client::Run()
	{
		SendInfo();

		switch (client_type)
		{
		case Instance::type::first_client_handler:
		{
			std::cout << "first\n";
		} break;

		case Instance::type::second_client_handler:
		{
			std::cout << "second\n";
		} break;

		default:
			std::cout << "dafault\n";
			break;
		}

		recv_thread = (std::thread(&Client::RecvMSG, this));

		while (is_connected)
		{
			SendMSG();
		}
	}

	Client::~Client()
	{
		recv_thread.join();
		WSACleanup();
		closesocket(client_socket);
	}
}	