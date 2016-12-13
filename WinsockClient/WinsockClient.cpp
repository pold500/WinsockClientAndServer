// WinsockClient.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Helpers.h"
#include "GeometryStructures.h"
#include "WSAClient.h"
#include <boost/algorithm/string.hpp>
#include <set>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
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

void parseCmdResponse(const std::string& userCmd, const std::string& rcvdPacket)
{
	std::vector<std::string> cmd_tokens;
	boost::split(cmd_tokens, userCmd, boost::is_any_of(", "));
	std::cout << rcvdPacket << "\n";
	if (cmd_tokens.size() >= 2 && cmd_tokens[0] == "get" && cmd_tokens[1] == "polygons" &&
		rcvdPacket.find("error") == std::string::npos) //yeah, I know it's shitty error handling mechanism.
	{
		std::stringstream str_stream;
		str_stream << rcvdPacket;
		cereal::BinaryInputArchive inputArchive(str_stream);
		ObjFileData fd;
		fd.load(inputArchive);
		//std::cout << str_stream.str() << "\n";
		std::vector<Point3D<float>> unique_vertices;
		for (auto poly : fd.m_polygons)
		{
			for (auto vertex : poly.m_polygonVertices)
			{
				if (std::find(begin(unique_vertices), end(unique_vertices), vertex) == end(unique_vertices))
					unique_vertices.push_back(vertex);
			}
		}
		fd.debugPrint();
	}
	else
	{
		std::cout << "Server response: \n" << rcvdPacket << "\n";
	}
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
	
	WSAClient client;

	const Helpers::Socket<SOCKET> ConnectionSocket = client.getClientSocket();

	if (ConnectionSocket == INVALID_SOCKET)
	{
		std::cout << "Please run a server first \n";
		std::cout << "Press any key to close \n";
		std::cin.get();
		return -1;
	}

	while (true)
	{
		std::cout << "Please enter your command: \n";
		const auto userCmd = getCmd();//std::string("get polygons cube.obj 0,1,2,3,4,5,6,7,8,9,10,11,12");//getCmd();
		if (!userCmd.empty())
		{
			
			try 
			{
				Helpers::sendPacket(ConnectionSocket, userCmd);
				std::cout << "User request is : \"" << userCmd << "\"\n";

				const auto rcvdPacket = Helpers::receivePacket(ConnectionSocket);
				
				parseCmdResponse(userCmd, rcvdPacket);
				
			}
			catch (const std::exception& ex)
			{
				std::cout << "Client caught an exception : " << ex.what() << "\n";
			}
		}
	}

	std::string a;
	std::cin >> a;

	return 0;
}
