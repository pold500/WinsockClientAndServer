#include "stdafx.h"
#include "ListCommand.h"
#include "Helpers.h"

void ListFilesUserCommand::execute()
{
	std::string listOfFilesAvailable = "Objects available for request: \n";

	std::for_each(begin(m_namesList), end(m_namesList), [&listOfFilesAvailable]
	(decltype(m_namesList[0])& name)
	{
		listOfFilesAvailable += name + "\n";
	});
	Helpers::sendPacket(m_ClientSocket, listOfFilesAvailable);
}
