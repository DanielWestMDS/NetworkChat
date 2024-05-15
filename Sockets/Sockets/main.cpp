#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>
#include <cstring>

#define BUFFER_SIZE 256

char g_cPutStr[255];
char g_cCapitalStr[255];

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

	//char input;
	//std::cout << "input 1 for server 2 for client: ";
	//std::cin >> input;

	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cout << "Sock Err: " << WSAGetLastError();
		WSACleanup();
		return 0;
	}

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

	//// listen()
	//if (!listen(sock))
	//{
	//	printf("Error in listen(). Code: %d\n", WSAGetLastError());
	//	WSACleanup();
	//	return 0;
	//}
	printf("Listening. . . \n");
	int status = listen(sock, 5);
	if (status == SOCKET_ERROR)
	{
		printf("Error in listen(). Code: %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	printf("accepting. . .\n");
	sockaddr_in cliAddr;
	int addrLen = sizeof(cliAddr);
	SOCKET cliSock = accept(sock, (sockaddr*)&cliAddr, &addrLen);
	if (cliSock == INVALID_SOCKET)
	{
		printf("Error in listen(). Code: %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	//return 1;

	// recieve
	char clientName[13];
	printf("got connection from client %s \n", inet_ntop(AF_INET, &cliAddr.sin_addr,
		clientName, sizeof(clientName)));	printf("receiving. . . \n");
	char buffer[BUFFER_SIZE];
	while (true)
	{
		printf("waiting for a message. . . \n");
		int rcv = recv(cliSock, buffer, 256, 0);
		if (rcv == SOCKET_ERROR)
		{
			printf("Error in recieve(). Error Code %d\n", WSAGetLastError());
			continue;
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
				printf("Message recieved\n");
			}

			// check for /GET command
			if (strstr(buffer, "/GET"))
			{
				printf("Put message: %s\n\n", g_cPutStr);
			}

			// check for /CAPITALIZE command
			if (strstr(buffer, "/CAPITALIZE"))
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
				printf("Capitalized message: %s\n\n", g_cCapitalStr);
			}
		}

	}

	// close server if all all hosts disconnected
	closesocket(sock);
}