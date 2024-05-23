#include "CClient.h"

CClient::CClient()
{
}

CClient::~CClient()
{
	closesocket(m_clientSock);
}

int CClient::Setup()
{
	m_clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_clientSock == INVALID_SOCKET)
	{
		printf("Error in socket(), error: %d\n", WSAGetLastError());
		return 0;
	}

	//set address
	m_clientAddr.sin_family = AF_INET;
	m_clientAddr.sin_port = htons(54672);
	m_clientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//bind 
	m_iStatus = bind(m_clientSock, (sockaddr*)&m_clientAddr, sizeof(m_clientAddr));
	if (m_iStatus == SOCKET_ERROR)
	{
		printf("Error in bind(), Error: %d\n", WSAGetLastError());
		return 0;
	}

	sockaddr_in recAddr;
	recAddr.sin_family = AF_INET;
	recAddr.sin_port = htons(12031);
	InetPton(AF_INET, L"192.168.1.17", &recAddr.sin_addr.S_un.S_addr);

	m_iStatus = connect(m_clientSock, (sockaddr*)&recAddr, sizeof(recAddr));
	if (m_iStatus == SOCKET_ERROR)
	{
		printf("Error in connect() Error: %d\n", WSAGetLastError());
		return 0;
	}

	return 1;
}

void CClient::SendLoop()
{
	while (true)
	{
		printf("Enter a message to send: \n");
		// can I just use normal cin >> here?
		std::cin.getline(buffer, BUFFER_SIZE);

		// close socket if QUIT command called
		if (strstr(buffer, "/QUIT"))
		{
			shutdown(m_clientSock, SD_SEND);
			closesocket(m_clientSock);
			break;
		}

		// error checking
		m_iStatus = send(m_clientSock, buffer, strlen(buffer), 0);
		if (m_iStatus == SOCKET_ERROR)
		{
			printf("Error in send(). Error: %d\n", WSAGetLastError());
			break;
		}
	}
}
