#pragma once
#include "Helpers.h"

class WSAClient
{
	SOCKET m_socket;
public:
	WSAClient() {}
	SOCKET getClientSocket();
	~WSAClient();
};

