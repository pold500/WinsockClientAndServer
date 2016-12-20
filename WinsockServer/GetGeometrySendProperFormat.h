#ifndef GetGeometrySendProperFormat_h__
#define GetGeometrySendProperFormat_h__
#include "UserCommand.h"
class GetGeometrySendProperFormat : public UserCommand
{
	SOCKET m_socket;
public:
	GetGeometrySendProperFormat(SOCKET socket);
	virtual void execute();
};

#endif // GetGeometrySendProperFormat_h__


