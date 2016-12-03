// WinsockServer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
//Server:
//Initialize Winsock.
//Create a socket.
//Bind the socket.
//Listen on the socket for a client.
//Accept a connection from a client.
//Receive and send data.
//Disconnect.

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

/* simple multithreaded output */
class mt_output
{
};

template<typename T>
std::ostream& operator<<(const mt_output &class_instance, const T& output)
{
	static std::mutex m_print_output_mutex;
	std::lock_guard<std::mutex> lock(m_print_output_mutex);
	std::ostringstream newOstream;
	std::cout << output;
	return std::cout;
}

static mt_output _mt_output_instance;
#define console_log _mt_output_instance


template<typename T>
using custom_deleter_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

//TODO: make this a class member later
static std::set<int> m_clientTokensSet;

custom_deleter_unique_ptr<addrinfo> initWSA()
{
	WSADATA wsaData {0};
	int iResult = 0;

	SOCKET ListenSocket = INVALID_SOCKET;

	addrinfo *result = nullptr;
	addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return nullptr;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return nullptr;
	}
	return custom_deleter_unique_ptr<addrinfo>(result, [](addrinfo* f) { freeaddrinfo(f); });
}

int CreateClientToken(std::set<int>& client_tokens)
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

SOCKET createListenSocket(addrinfo* result)
{
	// Create a Listening SOCKET for connecting to server
	SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return INVALID_SOCKET;
	}

	// Setup the TCP listening socket
	int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}

	return ListenSocket;
}
/*
There are several different programming techniques using Winsock that can be used to
listen for multiple client connections. One programming technique is to create a continuous
loop that checks for connection requests using the listen function (see Listening on a Socket).
If a connection request occurs, the application calls the accept, AcceptEx, or WSAAccept function
and passes the work to another thread to handle the request. Several other programming techniques
are possible.
*/
void worker_thread_function(const SOCKET ClientSocket, const int threadClientToken)
{
	console_log << "starting client thread, client token is: " << threadClientToken << "\n";
	//worker thread
	while (true)
	{
		std::string recvbuf;
		recvbuf.resize(DEFAULT_BUFLEN);
		const int rcvdBytesCount = recv(ClientSocket, &recvbuf[0], /*recvbuf.size()*/DEFAULT_BUFLEN, 0);
		if (rcvdBytesCount > 0) 
		{
			printf("Bytes received: %d\n", rcvdBytesCount);

			// Echo the buffer back to the sender
			std::vector<char> sendbuf;
			sendbuf.resize(500);
			const int sentBytesCount = send(ClientSocket, &sendbuf[0], sendbuf.size(), 0);
			if (sentBytesCount == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				break;
			}
			printf("Bytes sent: %d\n", sentBytesCount);
		}
		else if (rcvdBytesCount == 0 && WSAGetLastError() == 0)
		{
			printf("All data was trasmitted succesfully...\n");
			//break;
		}
		else if(WSAGetLastError()!=0)
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			console_log << "terminating client thread, client token was: " << threadClientToken << "\n";
			closesocket(ClientSocket);
			break; //stop listening. end that thread
		}
	}
	closesocket(ClientSocket);
}



int main(void)
{
	srand(static_cast<unsigned int>(time(NULL)));
	console_log << "Server started \n";
	const auto addrInfo = initWSA();
	
	auto func_thread_deleter = [](std::thread* thread) { thread->detach(); };
	
	using thread_ptr = custom_deleter_unique_ptr<std::thread>;

	std::vector<thread_ptr> m_clientProcessingThreads;
	
	SOCKET ListenSocket = createListenSocket(addrInfo.get());
	
	while (true)
	{
		console_log << "Started ot accept connections from clients... \n";
		
		SOCKET ClientSocket = accept(ListenSocket, NULL, NULL); //blocking operation
		
		if (ListenSocket == INVALID_SOCKET)
		{
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			//reinit the listening socket and start listening again
			ListenSocket = createListenSocket(addrInfo.get());
			continue;
		}
		//if (ClientSocket == INVALID_SOCKET)
		//{
		//	printf("client socked failed with error: %d\n", WSAGetLastError());
		//	closesocket(ClientSocket);
		//	continue;
		//}
		console_log << "We got a connecting client!... \n";
		// if no longer need the listen socket
		auto threadFunc = [&ClientSocket]() { worker_thread_function(ClientSocket, CreateClientToken(m_clientTokensSet)); };
		m_clientProcessingThreads.push_back(thread_ptr(new std::thread(threadFunc), func_thread_deleter));
	}
	

	// cleanup
	
	WSACleanup();

	return 0;
}