#ifndef SendLargeFileCmd_h__
#define SendLargeFileCmd_h__


#include "UserCommand.h"
class SendLargeFileCmd: public UserCommand
{
	SOCKET m_socket;
public:
	SendLargeFileCmd(SOCKET socket) : m_socket(socket)
	{

	}
	

	virtual void execute() override;

};
#endif // SendLargeFileCmd_h__

