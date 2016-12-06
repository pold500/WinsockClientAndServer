#pragma once
#include "UserCommand.h"
class GetGeometrySendProperFormat: public UserCommand
{
	SOCKET m_socket;
public:
	GetGeometrySendProperFormat(SOCKET socket);
	virtual void execute() override;
};

