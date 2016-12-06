#include "stdafx.h"
#include "WSASocket.h"
#include "Helpers.h"
#include "Server.h"
#pragma comment (lib, "Ws2_32.lib")

int main(void)
{
	srand(static_cast<unsigned int>(time(NULL)));
	console_log << "Server started \n";
	auto wsaSocket = std::make_unique<WSASocketC>();
	auto server = std::make_unique<Server>(wsaSocket.get());
	
	while (true)
	{
		server->Update();
	}

	return 0;
}