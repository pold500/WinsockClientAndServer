#pragma once
#ifndef UserCommandNoOperation_h__
#define UserCommandNoOperation_h__
#include "UserCommand.h"

class UserCommandNoOperation : public UserCommand
{
	SOCKET m_ClientSocket;
public:
	UserCommandNoOperation(SOCKET clientSocket) : 
	m_ClientSocket(clientSocket){}
	virtual void execute();
};


#endif // UserCommandNoOperation_h__