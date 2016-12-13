#ifndef WSAClient_h__
#define WSAClient_h__
#include "Helpers.h"

class WSAClient
{
	SOCKET m_socket;
public:
	WSAClient() {}
	SOCKET getClientSocket();
	~WSAClient();
};

#endif // WSAClient_h__


