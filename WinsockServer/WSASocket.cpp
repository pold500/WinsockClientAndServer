#include "stdafx.h"
#include "WSASocket.h"
#include <WinSock2.h>

const char* WSASocketC::DEFAULT_PORT = "27015";

void WSASocketC::initWSA()
{
	WSADATA wsaData{ 0 };
	int iResult = 0;

	SOCKET ListenSocket = INVALID_SOCKET;
	addrinfo* addr_info = nullptr;
	addrinfo hints { 0 };

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		throw std::exception();
	}
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addr_info);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		throw std::exception();
	}
	m_addr_info = Helpers::custom_deleter_unique_ptr<addrinfo>(addr_info, [](addrinfo* f) { freeaddrinfo(f); });
}



Helpers::Socket<SOCKET> WSASocketC::createListenSocket() const
{
	// Create a Listening SOCKET for connecting to server
	SOCKET ListenSocket = socket(m_addr_info->ai_family, m_addr_info->ai_socktype, m_addr_info->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return INVALID_SOCKET;
	}

	// Setup the TCP listening socket
	int iResult = bind(ListenSocket, m_addr_info->ai_addr, (int)m_addr_info->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}

	return ListenSocket;
}


WSASocketC::WSASocketC()
{
	initWSA();
}

WSASocketC::~WSASocketC()
{
	::WSACleanup();
}
