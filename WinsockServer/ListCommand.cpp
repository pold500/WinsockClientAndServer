#include "stdafx.h"
#include "ListCommand.h"
#include "Helpers.h"

void ListFilesUserCommand::execute()
{
	std::string listOfFilesAvailable = "Objects available for request: \n";

	for (size_t i = 0; i < m_fileList.size(); i++)
	{
		listOfFilesAvailable += m_fileList[i].name + " polyCount: " + std::to_string(m_fileList[i].polygonCount) + " \n";
	}
	
	Helpers::sendPacket(m_ClientSocket, listOfFilesAvailable);
}
