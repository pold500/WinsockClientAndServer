#include "stdafx.h"
#include "SendLargeFileCmd.h"
#include <fstream>

void SendLargeFileCmd::execute()
{
	std::ifstream file("devyat_printsev_ambera_1.txt");
	std::string str;
	std::string file_contents;
	while (std::getline(file, str))
	{
		file_contents += str;
		file_contents.push_back('\n');
	}
	size_t bytes_in_file = file_contents.length();
	Helpers::sendPacket(m_socket, file_contents);
}


