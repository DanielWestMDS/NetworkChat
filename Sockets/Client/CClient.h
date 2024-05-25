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
	// wherer message is held
	char m_cBuffer[BUFFER_SIZE];
	// for getting IP address
	wchar_t m_wIP[BUFFER_SIZE];
	char m_cIP[BUFFER_SIZE];

	// constexpr for ports
	static constexpr int DEFAULT_PORT = 12031;
	static constexpr int BASE_PORT = 54672;
};

