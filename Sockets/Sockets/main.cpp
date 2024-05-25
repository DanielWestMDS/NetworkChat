// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) Media Design School
//
// File Name : main.cpp
// Description : main file for server. Hosts a server socket and gets multiple client sockets through threads. 
//				 Uses condition variable and atomic bool to close if all clients disconnect
// Author : Daniel West
// Mail : daniel.west@mds.ac.nz

#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>
#include <cstring>
#include <algorithm>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>

#define BUFFER_SIZE 256

// strings for commands
char g_cPutStr[255];
char g_cCapitalStr[255];

// number of active clients for threads
int g_iActiveClients = 0;
// mutex to check if all clients disconnected
std::mutex g_ClientMutex;
// condition variable to check if all clients disconnected
std::condition_variable g_ConditionVar;
// atomic bool for closing server after all clients disconnected
std::atomic<bool> g_bServerRunning = true;

// vector containg all connected clients
std::vector<SOCKET> g_ConnectedClients;

bool InitWSA()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	int result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0)
	{
		printf("WSA startup failed %d\n", result);
		// exit because result failed
		return false;
	}

	// check if version exists
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		printf("Error: Version is not available\n");
		// exit because version does not exist
		return false;
	}
	printf("WSA initialised successfully\n");
	return true;
}

void ManageClient(SOCKET cliSock)
{
	// add new socket to the list of connected clients
	g_ConnectedClients.push_back(cliSock); 

	// Notify all clients about the new client
	for (SOCKET& clientSock : g_ConnectedClients)
	{
		// exclude the client just added
		if (clientSock != cliSock) 
		{
			std::string sMessage = "New client joined the server.\n";
			send(clientSock, sMessage.c_str(), sMessage.size(), 0);
		}
	}

	// brackets so lock goes out of scope after increasing clients
	{
		std::lock_guard<std::mutex> lock(g_ClientMutex);
		g_iActiveClients++;
	}
	// recieve()
	char buffer[BUFFER_SIZE];
	sockaddr_in cliAddr;
	int addrLen = sizeof(cliAddr);
	getpeername(cliSock, (sockaddr*)&cliAddr, &addrLen);
	char clientName[13];
	printf("got connection from client %s \n", inet_ntop(AF_INET, &cliAddr.sin_addr,
		clientName, sizeof(clientName)));
	inet_ntop(AF_INET, &cliAddr.sin_addr, clientName, sizeof(clientName));
	printf("receiving. . . \n");

	while (true)
	{
		printf("waiting for a message. . . \n");
		int rcv = recv(cliSock, buffer, 256, 0);
		if (rcv == SOCKET_ERROR)
		{
			printf("Error in recieve(). Error Code %d\n", WSAGetLastError());
			break;
		}

		// if rcv 0 then client has disconnected
		if (rcv == 0)
		{
			printf("Client disconnected\n");
			closesocket(cliSock);
			break;
		}

		// prevent 256 edge case from messing things up
		if (rcv < BUFFER_SIZE)
		{
			buffer[rcv] = '\0';
		}
		printf("msg: %s\n\n", buffer);

		// command code here
		if (buffer[0] == '/')
		{
			// check for /PUT command
			if (strstr(buffer, "/PUT"))
			{
				for (int i = 4; i < 255; i++)
				{
					g_cPutStr[i - 4] = buffer[i];
				}
				// display to server console
				printf("Message recieved\n");
				// display to client
				std::string sMessage = "Your secret is safe with the server.\n";
				send(cliSock, sMessage.c_str(), sMessage.size(), 0);
			}

			// check for /GET command
			else if (strstr(buffer, "/GET"))
			{
				// display to server console
				printf("Put message: %s\n\n", g_cPutStr);
				// display to client
				send(cliSock, g_cPutStr, BUFFER_SIZE - 1, 0);
			}

			// check for /CAPITALIZE command
			else if (strstr(buffer, "/CAPITALIZE"))
			{
				for (int i = 12; i < 255; i++)
				{
					// alter ascii within lowercase alphabet to be capital
					if (buffer[i] < 123 && buffer[i] > 96)
					{
						g_cCapitalStr[i - 12] = buffer[i] - 32;
					}
					// just write non lowercase letters like normal
					else
					{
						g_cCapitalStr[i - 12] = buffer[i];
					}
				}
				// display to server console
				printf("Capitalized message: %s\n\n", g_cCapitalStr);
				// display to client
				send(cliSock, g_cCapitalStr, BUFFER_SIZE - 1, 0);
			}

			// check for /LAMBDA command
			else if (strstr(buffer, "/LAMBDA"))
			{
				printf("The following text was produced with a lambda: \n\n");
				char cPowerStr[255];
				int iPowerNum = 0;
				// wow look auto c++ 11 feature please please please
				for (auto number : buffer)
				{
					// lambda c++ 11 feature please please please
					cPowerStr[iPowerNum] = [](int number)->int {return number * number;}(number);
					iPowerNum++;
				}

				printf("Power of %s: %s\n\n", buffer, cPowerStr);
			}
			else
			{
				printf("INVALID COMMAND \n\n");
			}
		}
		else
		{
			// send non commands to all clients
			for (SOCKET& clientSock : g_ConnectedClients)
			{
				send(clientSock, buffer, BUFFER_SIZE - 1, 0);
			}
		}
	}

	// alter active clients if client disconnects
	{
		std::lock_guard<std::mutex> lock(g_ClientMutex);
		g_iActiveClients--;
		if (g_iActiveClients == 0)
		{
			// set condition variable if all clients disconnect
			g_ConditionVar.notify_all();
			// stop server running if no clients
			g_bServerRunning = false;
		}
	}
}

int main()
{
	// initialise WSA
	InitWSA();

	// create server socket
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cout << "Sock Err: " << WSAGetLastError();
		WSACleanup();
		return 0;
	}

	// set server socket address
	sockaddr_in sockAddr{};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(12031);
	sockAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// bind socket
	int bound = bind(sock, (sockaddr*)&sockAddr, sizeof(sockAddr));
	if (bound == SOCKET_ERROR)
	{
		std::cout << "Bind Err: " << WSAGetLastError();
		WSACleanup();
		return 0;
	}

	// listen()
	printf("Listening. . . \n");
	int status = listen(sock, 5);
	if (status == SOCKET_ERROR)
	{
		printf("Error in listen(). Code: %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	printf("accepting. . .\n");
	while (g_bServerRunning)
	{
		// for montoring socket
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);

		// timeout duration for select()
		timeval timeout;
		// timeout set to one second
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		// select()
		int activity = select(0, &readfds, NULL, NULL, &timeout);
		if (activity == SOCKET_ERROR)
		{
			printf("Select error: %d\n", WSAGetLastError());
			break;
		}

		if (activity > 0 && FD_ISSET(sock, &readfds))
		{
			sockaddr_in cliAddr;
			int addrLen = sizeof(cliAddr);
			SOCKET cliSock = accept(sock, (sockaddr*)&cliAddr, &addrLen);
			if (cliSock == INVALID_SOCKET)
			{
				// report error is server is supposed to be running
				if (g_bServerRunning) 
				{
					printf("Accept error: %d\n", WSAGetLastError());
				}
				continue;
			}

			std::thread clientThread(ManageClient, cliSock);
			clientThread.detach();
		}
	}
	
	// wait for all clients to disconnect
	std::unique_lock<std::mutex> lock(g_ClientMutex);
	// lambda !!! for if all clients disconnect
	g_ConditionVar.wait(lock, [] { return g_iActiveClients == 0; });

	// close server if all all hosts disconnected
	closesocket(sock);
	WSACleanup();
	return 0;
}