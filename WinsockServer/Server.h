#ifndef __C_SERVER_HEADER__
#define __C_SERVER_HEADER__
#pragma once
#include <vector>
#include "Helpers.h"

class WSASocketC;
using thread_ptr = custom_deleter_unique_ptr<std::thread>;

class Server
{
	std::set<int> m_clientTokensSet;
	std::vector<thread_ptr> m_clientProcessingThreads;
	Socket<SOCKET> m_listenSocket;
	WSASocketC* m_wsaSocket;
	void worker_thread_function(const Socket<SOCKET> ClientSocket, const int threadClientToken);
	int createClientToken(std::set<int>& client_tokens);
public:
	Server(WSASocketC* socketClass);
	void Update();

};

#endif
