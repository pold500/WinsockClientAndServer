// WinsockClient.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <string>
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

SOCKET InitConnectionSocket()
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	
	//char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) 
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return INVALID_SOCKET;
	}
	return ConnectSocket;
}

int shutDownSocket(const SOCKET socket)
{
	// shutdown the connection since no more data will be sent
	const int iResult = shutdown(socket, SD_SEND);
	if (iResult == SOCKET_ERROR) 
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	std::cout << "Client started \n";
	auto getCmd = []()->std::string
	{
		std::string str;
		std::getline(std::cin, str);
		return str;
	};
	
	const SOCKET ConnectSocket = InitConnectionSocket();

	if (ConnectSocket == INVALID_SOCKET)
	{
		std::cout << "Please run a server first \n";
		std::cout << "Press any key to close \n";
		std::cin.get();
		return -1;
	}

	while (true)
	{
		std::cout << "Please enter your command: \n";
		const auto userCmd = getCmd();
		if (!userCmd.empty())
		{
			// Send an initial buffer
			
			int bytesReceived = send(ConnectSocket, userCmd.c_str(), userCmd.size(), 0);
			if (bytesReceived == SOCKET_ERROR) 
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}

			printf("Bytes Sent: %ld\n", bytesReceived);
			
			// Receive until the peer closes the connection
			
			std::vector<char> recvbuf;
			recvbuf.resize(500);
			
			do 
			{
				bytesReceived = recv(ConnectSocket, &recvbuf[0], recvbuf.size(), 0);
				if (bytesReceived > 0)
				{
					printf("Bytes received: %d\n", bytesReceived);
				}
				else if (bytesReceived == 0)
				{
					printf("Connection closed\n");
					break;
				}
				else
				{
					printf("recv failed with error: %d\n", WSAGetLastError());
				}
			} 
			while (bytesReceived == 0);
		}
	}
	
	// cleanup
	shutDownSocket(ConnectSocket);
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
