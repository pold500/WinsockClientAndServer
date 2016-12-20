#include "stdafx.h"
#include "GetGeometrySendProperFormat.h"
#include "Helpers.h"

GetGeometrySendProperFormat::GetGeometrySendProperFormat(SOCKET socket)
	: m_socket(socket)
{

}


void GetGeometrySendProperFormat::execute()
{
	static const std::string get_command_format = "get <polygons> <filename.obj> <1..n|1,2,..,n>," +
		std::string("n should be less then count of object polygons.") +
		std::string("you can get list of possible filenames by issuing \"list\" command");
	Helpers::CStringPacket stringPacket(get_command_format);
	
	if (!Helpers::sendPacket(m_socket, stringPacket))
	{
		//handle disconnection
	}
}
