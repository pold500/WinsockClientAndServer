#include "stdafx.h"
#include "UserCommandNoOperation.h"
#include "Helpers.h"

void UserCommandNoOperation::execute()
{
	static const std::string no_such_command_message = "No such command exist. " +
		std::string("List of available commands: list, get polygons [start_range..end_range], get polygons [indexes] \n");
	Helpers::sendPacket(m_ClientSocket, no_such_command_message);
}

