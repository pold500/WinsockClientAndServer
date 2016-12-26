#ifndef StringResponseCmd_h__
#define StringResponseCmd_h__
#include "UserCommand.h"
#include <string>

class StringResponseCmd: public UserCommand
{
	std::string m_response;
	int m_socket;
public:
	StringResponseCmd(const int socket, const std::string& response);
	virtual void execute();
};
#endif // StringResponseCmd_h__

