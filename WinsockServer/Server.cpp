#include "stdafx.h"
#include "Server.h"
#include "WSASocket.h"

const int DEFAULT_BUFLEN = 512;

Server::Server(WSASocketC* wsaSocket):
	m_wsaSocket(wsaSocket),
	m_listenSocket(wsaSocket->createListenSocket())
{

}

/*
There are several different programming techniques using Winsock that can be used to
listen for multiple client connections. One programming technique is to create a continuous
loop that checks for connection requests using the listen function (see Listening on a Socket).
If a connection request occurs, the application calls the accept, AcceptEx, or WSAAccept function
and passes the work to another thread to handle the request. Several other programming techniques
are possible.
*/
void Server::worker_thread_function(const Socket<SOCKET> ClientSocket, const int threadClientToken)
{
	console_log << "starting client thread, client token is: " << threadClientToken << "\n";
	//worker thread
	
	while (true)
	{
		std::string recvbuf;
		recvbuf.resize(DEFAULT_BUFLEN);
		const int rcvdBytesCount = ::recv(ClientSocket, &recvbuf[0], /*recvbuf.size()*/DEFAULT_BUFLEN, 0);
		if (rcvdBytesCount > 0)
		{
			printf("Bytes received: %d\n", rcvdBytesCount);

			// Echo the buffer back to the sender
			std::vector<char> sendbuf;
			sendbuf.resize(500);

			const int sentBytesCount = ::send(ClientSocket, &sendbuf[0], sendbuf.size(), 0);
			if (sentBytesCount == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				/*closesocket(ClientSocket);*/
				break;
			}
			printf("Bytes sent: %d\n", sentBytesCount);
		}
		else if (rcvdBytesCount == 0 && WSAGetLastError() == 0)
		{
			printf("All data was trasmitted succesfully...\n");
			//break;
		}
		else if (WSAGetLastError() != 0)
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			console_log << "terminating client thread, client token was: " << threadClientToken << "\n";
			/*closesocket(ClientSocket);*/
			break; //stop listening. end that thread
		}
	}
	/*closesocket(ClientSocket);*/
}



void Server::Update()
{
	console_log << "Started ot accept connections from clients... \n";
	
	SOCKET ClientSocket = accept(m_listenSocket.getSocket(), NULL, NULL); //blocking operation

	if (m_listenSocket.getSocket() == INVALID_SOCKET)
	{
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		//reinit the listening socket and start listening again
		while((m_listenSocket = m_wsaSocket->createListenSocket()).getSocket() == INVALID_SOCKET);
		
	}

	console_log << "We got a connecting client!... \n";
	auto threadFunc = [&ClientSocket, this]() { this->worker_thread_function(ClientSocket, createClientToken(m_clientTokensSet)); };
	auto func_thread_deleter = [](std::thread* thread) { thread->detach(); };
	m_clientProcessingThreads.push_back(thread_ptr(new std::thread(threadFunc), func_thread_deleter));
}

int Server::createClientToken(std::set<int>& client_tokens)
{
	const int max_clients = 1000;
	auto getNewToken = [&max_clients]()->int { return rand() % max_clients; };
	int newToken = getNewToken();
	while (client_tokens.count(newToken))
	{
		newToken = getNewToken();
	}
	client_tokens.insert(end(client_tokens), newToken);
	return newToken;
}

