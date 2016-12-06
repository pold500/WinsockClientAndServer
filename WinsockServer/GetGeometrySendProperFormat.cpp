#include "stdafx.h"
#include "GetGeometrySendProperFormat.h"


GetGeometrySendProperFormat::GetGeometrySendProperFormat(SOCKET socket)
	: m_socket(socket)
{

}


void GetGeometrySendProperFormat::execute()
{
	static const std::string get_command_format = "get <polygons> <1..3|1,2,15>";
	Helpers::sendPacket(m_socket, get_command_format);
}
