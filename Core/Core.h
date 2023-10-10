#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include <iphlpapi.h>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#define INET_ADDRSTRLEN 10
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

#define DEBUG 0
#define RELEASE 0

namespace Instance
{
	enum class type : uint8_t
	{
		first_client_handler = 0,
		second_client_handler 
	};

	inline type client_handler;
}