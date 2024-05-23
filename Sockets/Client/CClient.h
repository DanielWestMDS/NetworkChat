#pragma once
#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>
#include <cstring>

#define BUFFER_SIZE 256

class CClient
{
public:
	CClient();
	~CClient();
	
	int Setup();
	void SendLoop();
private:
	// client socket
	SOCKET m_clientSock;
	// client socket address
	sockaddr_in m_clientAddr;
	// bind status
	int m_iStatus;
	char buffer[BUFFER_SIZE];
};

