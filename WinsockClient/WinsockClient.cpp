// WinsockClient.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Helpers.h"
#include "GeometryStructures.h"
#include "WSAClient.h"
#include <boost/algorithm/string.hpp>
#include <set>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

void writeReponseToFile(const std::string& response)
{
	std::ofstream out("out.txt");
	out << response;
	out.flush();
	out.close();
}

auto getCmd = []()->std::string
{
	std::string str;
	std::getline(std::cin, str);
	return str;
};

bool updateTask(SOCKET ConnectionSocket)
{
	while (true)
	{
		std::cout << "Please enter your command: \n";
		const auto userCmd = getCmd();
		if (!userCmd.empty())
		{
			try
			{
				Helpers::sendPacket(ConnectionSocket, userCmd);
				std::cout << "User request is : \"" << userCmd << "\"\n";
				const auto rcvdPacket = Helpers::receivePacket(ConnectionSocket);
				if (!rcvdPacket.is_initialized())
				{
					return false;
				}
				std::cout << *rcvdPacket << std::endl;
			}
			catch (const std::exception& ex)
			{
				std::cout << "Client caught an exception : " << ex.what() << "\n";
			}
		}
	}
}

int main(int argc, char **argv)
{
	std::cout << "Client started \n";
	
	WSAClient client;

	//const Helpers::Socket<SOCKET> ConnectionSocket = client.getClientSocket();

	while(const Helpers::Socket<SOCKET> ConnectionSocket = client.getClientSocket())
	{
		while (updateTask(ConnectionSocket.getSocket()));
	}

	return 0;
}
