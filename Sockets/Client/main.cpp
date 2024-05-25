// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) Media Design School
//
// File Name : main.cpp
// Description : main file for client. Initialises WSA and calls client class with a smart pointer. 
// Author : Daniel West
// Mail : daniel.west@mds.ac.nz

#include "CClient.h"
#include <memory>

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
		return false;
	}
	printf("WSA initialised successfully\n");
	return true;
}

int main()
{
	// initialise WSA
	InitWSA();

	// unique pointer so it is automatically deleted when it goes out of scope.
	std::unique_ptr<CClient> Client = std::make_unique<CClient>();

	if (Client->Setup())
	{
		// send client data to server
		Client->SendLoop();
	}

	WSACleanup();
	return 0;
}
