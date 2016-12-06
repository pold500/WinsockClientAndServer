#pragma once
#include "UserCommand.h"

class ListFilesUserCommand: public UserCommand
{
	std::vector<std::string> m_namesList;
	SOCKET m_ClientSocket;
public:
	ListFilesUserCommand(const SOCKET _socket, const std::vector<std::string>& fileList) :
		m_ClientSocket(_socket),
		m_namesList(fileList)
	{
	}
	virtual void execute() override;
	
};

