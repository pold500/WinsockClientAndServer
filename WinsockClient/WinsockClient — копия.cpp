// WinsockClient.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Helpers.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#undef max

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
			//Create user packet and send it
			const auto data_to_send = createPacket(userCmd);
			size_t totalBytesSent = 0;
			sendPacket(ConnectSocket, data_to_send);
			
			// Switch into receive mode
			{

				std::string recvbuf;
				const size_t rcv_buffer_length = 512;
				recvbuf.resize(rcv_buffer_length); //buffer is small - just to check that we receive all data in small chunks
				size_t packetLength = std::numeric_limits<size_t>::max();
				size_t bytesReceivedInCycle = 0;
				size_t totalBytesReceived = 0;
				std::string full_message;
				do
				{
					bytesReceivedInCycle = recv(ConnectSocket, &recvbuf[0], recvbuf.size(), 0);
					totalBytesReceived += bytesReceivedInCycle;
					if (recvbuf.length() >= 8)
					{
						//recvbuf.resize(bytesReceived);
						printf("Bytes received: %d\n", bytesReceivedInCycle);
						if (packetLength == -1)
						{
							packetLength = readSizePacket(recvbuf);
							full_message.reserve(packetLength);
						}
					}

					if (totalBytesReceived > 0)
					{
						full_message.append(recvbuf);
						recvbuf.clear();
						recvbuf.resize(rcv_buffer_length);
					}
					//error processing
					else if (bytesReceivedInCycle == 0)
					{
						printf("Connection closed\n");
						break;
					}
					else if(WSAGetLastError())
					{
						printf("recv failed with error: %d\n", WSAGetLastError());
					}
				} while (totalBytesReceived < packetLength);
				//full message has been received - we read all the data from the socket
				try
				{
					const auto base64_decoded = base64::decode(full_message.substr(sizeof(size_t)));
					std::string decoded_string(begin(base64_decoded), end(base64_decoded));
					std::cout << decoded_string << std::endl;
				}
				catch (std::exception& ex)
				{
					std::cout << "Caught an exception while trying to decode message.\n";
					std::cout << ex.what() << std::endl;
				}
				bytesReceivedInCycle = packetLength = 0;
			}
			
		}
	}
	
	// cleanup
	shutDownSocket(ConnectSocket);
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
