#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>

#define BUFFER_SIZE 256

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

int Client()
{
	SOCKET clientSock;
	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET)
	{
		printf("Error in socket(), error: %d\n", WSAGetLastError());
		return 0;
	}

	//set address
	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(54672);
	clientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//bind 
	int status = bind(clientSock, (sockaddr*)&clientAddr, sizeof(clientAddr));
	if (status == SOCKET_ERROR)
	{
		printf("Error in bind(), Error: %d\n", WSAGetLastError());
		return 0;
	}

	sockaddr_in recAddr;
	recAddr.sin_family = AF_INET;
	recAddr.sin_port = htons(12031);
	InetPton(AF_INET, L"127.0.0.1", &recAddr.sin_addr.S_un.S_addr);

	status = connect(clientSock, (sockaddr*)&recAddr, sizeof(recAddr));
	if (status == SOCKET_ERROR)
	{
		printf("Error in connect() Error: %d\n", WSAGetLastError());
		return 0;
	}

	char buffer[BUFFER_SIZE];

	while (true)
	{
		printf("Enter a message to send: \n");
		// can I just use normal cin >> here?
		std::cin.getline(buffer, BUFFER_SIZE);

		status = send(clientSock, buffer, strlen(buffer), 0);
		if (status == SOCKET_ERROR)
		{
			printf("Error in send(). Error: %d\n", WSAGetLastError());
			break;
		}
	}

	closesocket(clientSock);
}

int main()
{
	// initialise WSA
	InitWSA();

	Client();
	return 0;
}
