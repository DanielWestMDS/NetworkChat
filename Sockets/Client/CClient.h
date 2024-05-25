// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) Media Design School
//
// File Name : CClient.h
// Description : header file for client class. encapsulates member variables privately and setup and send loop functions publicly.
// Author : Daniel West
// Mail : daniel.west@mds.ac.nz

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
	// type alias using c++ 11 feature please please please
	using buffer = char[BUFFER_SIZE];

	// client socket
	SOCKET m_clientSock;
	// client socket address
	sockaddr_in m_clientAddr;
	// bind status
	int m_iStatus;
	// wherer message is held
	buffer m_cBuffer;
	// for getting IP address
	wchar_t m_wIP[BUFFER_SIZE];
	char m_cIP[BUFFER_SIZE];

	// constexpr for ports a c++ 11 feature please please please
	static constexpr int DEFAULT_PORT = 12031;
	static constexpr int BASE_PORT = 54672;
};

