#ifndef _WSA_SOCKET_CLASS
#define _WSA_SOCKET_CLASS
#pragma once
#include "Helpers.h"


class WSASocketC
{
	static const char* DEFAULT_PORT;
	Helpers::custom_deleter_unique_ptr<addrinfo> m_addr_info;
	void initWSA();
public:
	Helpers::Socket<SOCKET> createListenSocket() const;
	WSASocketC();
	~WSASocketC();
};
#endif

