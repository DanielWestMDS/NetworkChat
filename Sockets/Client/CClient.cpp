#include "CClient.h"

CClient::CClient()
{
	// initialise values
	m_clientSock = NULL;
	m_clientAddr.sin_family = NULL;
	m_clientAddr.sin_port = NULL;
	m_clientAddr.sin_addr.S_un.S_addr = NULL;	// bind status
	m_iStatus = 0;
	m_cBuffer[BUFFER_SIZE - 1] = {};
	m_wIP[BUFFER_SIZE - 1] = {};
	m_cIP[BUFFER_SIZE - 1] = {};
}

CClient::~CClient()
{
	shutdown(m_clientSock, SD_SEND);
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

	// iterate through sockets until one works
	int iSockOffset = 0;
	do
	{
		//set address
		m_clientAddr.sin_family = AF_INET;
		m_clientAddr.sin_port = htons(BASE_PORT + iSockOffset);
		m_clientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

		//bind 
		m_iStatus = bind(m_clientSock, (sockaddr*)&m_clientAddr, sizeof(m_clientAddr));
		if (m_iStatus == SOCKET_ERROR)
		{
			printf("Error in bind(), Error: %d\n", WSAGetLastError());
			iSockOffset++;
		}
	} while (m_iStatus == SOCKET_ERROR);

	sockaddr_in recAddr;
	recAddr.sin_family = AF_INET;
	recAddr.sin_port = htons(DEFAULT_PORT);
	printf("\nPlease Enter the Server IP address: ");
	std::cin.getline(m_cIP, BUFFER_SIZE);
	// convert IP to something InetPton can read (wchar_t)
	size_t iConvertedCharacters = 0;
	mbstowcs_s(&iConvertedCharacters, m_wIP, m_cIP, BUFFER_SIZE - 1);
	InetPton(AF_INET, m_wIP, &recAddr.sin_addr.S_un.S_addr);

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
		std::cin.getline(m_cBuffer, BUFFER_SIZE);

		// close socket if QUIT command called
		if (strstr(m_cBuffer, "/QUIT"))
		{
			shutdown(m_clientSock, SD_SEND);
			closesocket(m_clientSock);
			break;
		}

		// error checking
		m_iStatus = send(m_clientSock, m_cBuffer, strlen(m_cBuffer), 0);
		if (m_iStatus == SOCKET_ERROR)
		{
			printf("Error in send(). Error: %d\n", WSAGetLastError());
			break;
		}
	}
}
