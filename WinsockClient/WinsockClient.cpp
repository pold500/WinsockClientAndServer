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

Helpers::CStringPacket getCmd()
{
	std::string str;
	std::getline(std::cin, str);
	Helpers::CStringPacket strPacket(str);
	return strPacket;
};

bool updateTask(SOCKET ConnectionSocket)
{
	while (true)
	{
		std::cout << "Please enter your command: \n";
		const auto userCmd = getCmd();
		if (!userCmd.GetStringData().empty())
		{
			try
			{
				Helpers::sendPacket(ConnectionSocket, userCmd);
				const auto rcvdPacket = Helpers::receivePacket(ConnectionSocket);
				if (!rcvdPacket.get())
				{
					return false;
				}

				if (rcvdPacket->m_packetType == Helpers::CPacket::StringResponse)
				{
					std::cout << static_cast<const Helpers::CStringPacket*>(rcvdPacket.get())->GetStringData() << std::endl;
				}
				else if(rcvdPacket->m_packetType == Helpers::CPacket::BinaryVertexData_V2)//we have a binary vertex data
				{
					//save it to file
					auto packet = static_cast<Helpers::CBinaryVertexDataPacket_V2*>(rcvdPacket.get());
					Helpers::writeObjToFile("objFromServer.obj", packet->m_objectData->m_vertices, packet->m_objectData->m_faces);
				}
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
