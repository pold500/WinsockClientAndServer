// WinsockClient.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Helpers.h"
#include "GeometryStructures.h"
#include <boost/algorithm/string.hpp>

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

void writeReponseToFile(const std::string& response)
{
	std::ofstream out("out.txt");
	out << response;
	out.flush();
	out.close();
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
	
	const Helpers::Socket<SOCKET> ConnectSocket = InitConnectionSocket();

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
		const auto userCmd = std::string("get polygons cornell_box.obj 1,2,4,5,6");//getCmd();
		if (!userCmd.empty())
		{
			Helpers::sendPacket(ConnectSocket, userCmd);
			std::cout << "User request is : \"" << userCmd << "\"\n";
			try 
			{
				const auto rcvdPacket = Helpers::receivePacket(ConnectSocket);
				//std::cout << "Server response: \n";
				//std::cout << rcvdPacket << std::endl;
				
				std::vector<std::string> cmd_tokens;
				boost::split(cmd_tokens, userCmd, boost::is_any_of(", "));
				
				if (cmd_tokens.size() >= 2 && cmd_tokens[0] == "get" && cmd_tokens[1] == "polygons")
				{
					std::stringstream str_stream;
					str_stream << rcvdPacket;
					cereal::BinaryInputArchive inputArchive(str_stream);
					ObjFileData fd;
					fd.load(inputArchive);
					std::cout << str_stream.str() << "\n";
					fd.debugPrint();
				}
				//writeReponseToFile(rcvdPacket);
			}
			catch (const std::exception& ex)
			{
				std::cout << "Client caught an exception : " << ex.what() << "\n";
				std::cin.get();
			}
			break;
		}
	}
	std::string a;
	std::cin >> a;
	// cleanup
	shutDownSocket(ConnectSocket);
	WSACleanup();
	return 0;
}
