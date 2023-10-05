#include "../include/client.h"

#pragma warning(disable: 4996)


namespace UDPChat
{
	Client::Client(const char* ip)
		:
		wsa{ 0 },
		client_socket(INVALID_SOCKET),
		server_info{ 0 },
		server_info_lenght(sizeof(server_info)),
		client_info_ip(NULL),
		client_info{ 0 },
		client_info_lenght(sizeof(client_info)),
		is_connected(false),
		recv_message_size(0) 
	{
		client_info.sin_addr.s_addr = reinterpret_cast<u_short>(ip); 
	}


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

		char yes = '1';

		if ((setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1))
		{
			std::cout << WSAGetLastError() << '\n';
			perror("setsockport");
			return false;
		}

		server_info.sin_family = AF_INET;
		server_info.sin_port = htons(port);
		server_info.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

		ZeroMemory(server_info.sin_zero, 8);

		return true;
	}

	bool Client::GetIp()
	{
		PIP_ADAPTER_INFO pAdapterInfo;
		DWORD dwRetVal = 0;
		UINT i;

		/* variables used to print DHCP time info */
		struct tm newtime;
		char buffer[32];
		errno_t error;

		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
		if (pAdapterInfo == NULL)
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			return false;
		}
		// Make an initial call to GetAdaptersInfo to get
		// the necessary size into the ulOutBufLen variable
		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
		{
			FREE(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);

			if (pAdapterInfo == NULL)
			{
				printf("Error allocating memory needed to call GetAdaptersinfo\n");
				return false;
			}
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
		{
			client_info_ip = pAdapterInfo;

			while (client_info_ip)
			{
				printf("\tComboIndex: \t%d\n", client_info_ip->ComboIndex);
				printf("\tAdapter Name: \t%s\n", client_info_ip->AdapterName);
				printf("\tAdapter Desc: \t%s\n", client_info_ip->Description);
				printf("\tAdapter Addr: \t");

				for (i = 0; i < client_info_ip->AddressLength; i++) 
				{
					if (i == (client_info_ip->AddressLength - 1))
					{
						printf("%.2X\n", (int)client_info_ip->Address[i]);
					}
					else
					{
						printf("%.2X-", (int)client_info_ip->Address[i]);
					}
				}

				printf("\tIndex: \t%d\n", client_info_ip->Index);
				printf("\tType: \t");

				switch (client_info_ip->Type)
				{
				case MIB_IF_TYPE_OTHER:
					printf("Other\n");
					break;
				case MIB_IF_TYPE_ETHERNET:
					printf("Ethernet\n");
					break;
				case MIB_IF_TYPE_TOKENRING:
					printf("Token Ring\n");
					break;
				case MIB_IF_TYPE_FDDI:
					printf("FDDI\n");
					break;
				case MIB_IF_TYPE_PPP:
					printf("PPP\n");
					break;
				case MIB_IF_TYPE_LOOPBACK:
					printf("Lookback\n");
					break;
				case MIB_IF_TYPE_SLIP:
					printf("Slip\n");
					break;
				default:
					printf("Unknown type %ld\n", client_info_ip->Type);
					break;
				}

				printf("\tIP Address: \t%s\n", client_info_ip->IpAddressList.IpAddress.String);
				printf("\tIP Mask: \t%s\n", client_info_ip->IpAddressList.IpMask.String);
				printf("\tGateway: \t%s\n", client_info_ip->GatewayList.IpAddress.String);
				printf("\t***\n");

				if (client_info_ip->DhcpEnabled)
				{
					printf("\tDHCP Enabled: Yes\n");
					printf("\t  DHCP Server: \t%s\n", client_info_ip->DhcpServer.IpAddress.String);
					printf("\t  Lease Obtained: ");

					/* Display local time */
					error = _localtime32_s(&newtime, (__time32_t*)&client_info_ip->LeaseObtained);

					if (error)
					{
						printf("Invalid Argument to _localtime32_s\n");
					}
					else 
					{
						// Convert to an ASCII representation 
						error = asctime_s(buffer, 32, &newtime);
						if (error)
						{
							printf("Invalid Argument to asctime_s\n");
						}
						else
						{
							/* asctime_s returns the string terminated by \n\0 */
							printf("%s", buffer);
						}
					}

					printf("\t  Lease Expires:  ");
					error = _localtime32_s(&newtime, (__time32_t*)&client_info_ip->LeaseExpires);
					if (error)
					{
						printf("Invalid Argument to _localtime32_s\n");
					}
					else 
					{
						// Convert to an ASCII representation 
						error = asctime_s(buffer, 32, &newtime);

						if (error)
						{
							printf("Invalid Argument to asctime_s\n");
						}
						else
						{
							/* asctime_s returns the string terminated by \n\0 */
							printf("%s", buffer);
						}
					}
				}
				else
				{
					printf("\tDHCP Enabled: No\n");
				}

				if (client_info_ip->HaveWins) 
				{
					printf("\tHave Wins: Yes\n");
					printf("\t  Primary Wins Server:    %s\n", client_info_ip->PrimaryWinsServer.IpAddress.String);
					printf("\t  Secondary Wins Server:  %s\n", client_info_ip->SecondaryWinsServer.IpAddress.String);
				}
				else
				{
					printf("\tHave Wins: No\n");
				}

				client_info_ip = client_info_ip->Next;
				printf("\n");
			}
		}
		else 
		{
			printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
		}

		if (pAdapterInfo)
			FREE(pAdapterInfo);

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
		GetIp();

		if (sendto(client_socket, (char*)&is_connected, sizeof(bool), 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto first client ip");
			return false;
		}

		client_info_lenght = sizeof(client_info);

		//getpeername(client_socket, (sockaddr*)&client_info, &client_info_lenght);
		//getsockname(client_socket, (sockaddr*)&client_info, &client_info_lenght);

		printf("Client %s:%d is connected to UDP server %s:%d\n", 
				inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port), 
				inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));

		if (sendto(client_socket, inet_ntoa(client_info.sin_addr), INET_ADDRSTRLEN, 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto first client ip");
			return false;
		}

		if (sendto(client_socket, (char*)&(client_info.sin_port), sizeof(int), 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
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

		if (sendto(client_socket, (char*)&send_message_size, sizeof(int), 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto message size");
			return;
		}

		if (sendto(client_socket, send_message.c_str(), send_message_size, 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
		{
			std::cout << WSAGetLastError() << '\n';
			perror("sendto message");
			return;
		}

		int client = static_cast<int>(client_type);

		if (sendto(client_socket, (char*)&client, sizeof(int), 0, (const sockaddr*)&server_info, server_info_lenght) <= 0)
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

			if (recvfrom(client_socket, (char*)&recv_message_size, sizeof(int), 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
			{
				std::cout << WSAGetLastError() << '\n';
				perror("recvfrom message size");
				return;
			}

			recv_message = new char[recv_message_size + 1];
			recv_message[recv_message_size] = '\0';

			if (recvfrom(client_socket, recv_message, recv_message_size, 0, (sockaddr*)&server_info, &server_info_lenght) <= 0)
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