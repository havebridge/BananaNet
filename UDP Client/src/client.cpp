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
		is_connected(false) {}


	bool Client::Connect(std::string ip, int port)
	{
		if ((WSAStartup(MAKEWORD(2, 2), &wsa)) == -1)
		{
			perror("WSAStartup");
			return false;
		}

		if ((client_socket = socket(PF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
		{
			perror("socket");
			return false;
		}

		char yes = '1';

		if ((setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1))
		{
			perror("setsockport");
			return false;
		}

		client_info.sin_family = AF_INET;
		client_info.sin_port = htons(port);
		client_info.sin_addr.s_addr = inet_addr(ip.c_str());

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
		if (sendto(client_socket, inet_ntoa(client_info.sin_addr), INET_ADDRSTRLEN, 0, (sockaddr*)&client_info, client_info_lenght) <= 0)
		{
			perror("sendto first client ip");
			return false;
		}

		int port = client_info.sin_port;

		if (sendto(client_socket, (char*)&port, sizeof(int), 0, (sockaddr*)&client_info, client_info_lenght) <= 0)
		{
			perror("sendto first client port");
			return false;
		}

		is_connected = true;

		ProcessFile("C:/Users/meylor/source/repos/hnet/handler/file_handler.txt");

		return is_connected;
	}

	void Client::SendMSG()
	{
		std::cout << "Send message: ";
		std::getline(std::cin, send_message);

		send_message_size = send_message.size();

		if (sendto(client_socket, (char*)&send_message_size, sizeof(int), 0, (const sockaddr*)&client_info, client_info_lenght) <= 0)
		{
			perror("sendto message size");
			return;
		}

		if (sendto(client_socket, send_message.c_str(), send_message_size, 0, (const sockaddr*)&client_info, client_info_lenght) <= 0)
		{
			perror("sendto message");
			return;
		}

		int client = static_cast<int>(client_type);

		if (sendto(client_socket, (char*)&client, sizeof(int), 0, (const sockaddr*)&client_info, client_info_lenght) <= 0)
		{
			perror("sendto message");
			return;
		}

		std::cout << "message size: " << send_message_size;
		std::cout << "\nmessage:" << send_message.data();
		std::cout << "\nclient type:" << client;
		std::cout << '\n';
	}

	void Client::RecvMSG()
	{
		recv_message_size = 0;

		if (recvfrom(client_socket, (char*)&recv_message_size, sizeof(int), 0, (sockaddr*)&client_info, &client_info_lenght) <= 0)
		{
			perror("recvfrom message size");
			return;
		}

		recv_message = new char[recv_message_size + 1];
		recv_message[recv_message_size] = '\0';

		if (recvfrom(client_socket, recv_message, recv_message_size, 0, (sockaddr*)&client_info, &client_info_lenght) <= 0)
		{
			perror("recvfrom message");
			return;
		}

		int client = 0;

		if (recvfrom(client_socket, (char*)&client, sizeof(int), 0, (sockaddr*)&client_info, &client_info_lenght) <= 0)
		{
			perror("recvfrom message");
			return;
		}

		std::cout << "message size recv: " << recv_message_size;
		std::cout << "\nmessage recv: " << recv_message;
		std::cout << "\nclient recv: " << client;
		std::cout << '\n';

		delete[] recv_message;
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

		std::thread recv_thread = std::thread(&Client::RecvMSG, this);

		while (is_connected)
		{
			SendMSG();
		}

		recv_thread.join();
	}


	/*void Client::Send()
	{
		std::cout << "Enter a message: ";
		std::getline(std::cin, message);
	

		if ((sendlength = sendto(clientsocket, message.c_str(), message.size(), 0, (struct sockaddr*) & info, infolength)) == 0)
		{
			perror("send");
			return;
		}
	}

	void Client::Recieve()
	{
		if ((recvlength = recvfrom(clientsocket, buffer, SIZE, 0, (struct sockaddr*) & info, &infolength)) == 0)
		{
			perror("recvfrom");
			return;
		}
	}


	void Client::Process()
	{
		std::cout << "Got the packet from:" << inet_ntoa(info.sin_addr) << ":" << htons(info.sin_port) << '\n';
		std::cout << "Data:";

		for (int i = 0; i != recvlength; ++i)
		{
			std::cout << buffer[i];
		}
		std::cout << '\n';
	}*/


	Client::~Client()
	{
		WSACleanup();
		closesocket(client_socket);
	}
}	