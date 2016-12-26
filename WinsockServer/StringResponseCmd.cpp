#include "StringResponseCmd.h"
#include "Helpers.h"

StringResponseCmd::StringResponseCmd(const int socket, const std::string& response):
	m_socket(socket),
	m_response(response)
{
}

void StringResponseCmd::execute()
{
	Helpers::sendPacket(m_socket, Helpers::CStringPacket(m_response));
}

