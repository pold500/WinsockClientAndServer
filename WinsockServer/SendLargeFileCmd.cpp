#include "stdafx.h"
#include "Helpers.h"
#include "SendLargeFileCmd.h"
#include <fstream>

void SendLargeFileCmd::execute()
{
	std::ifstream file("bigFile.txt");
	std::string str;
	std::string file_contents;
	while (std::getline(file, str))
	{
		file_contents += str;
		file_contents.push_back('\n');
	}
	size_t bytes_in_file = file_contents.length();
	Helpers::sendPacket(m_socket, Helpers::CStringPacket(file_contents));
}


