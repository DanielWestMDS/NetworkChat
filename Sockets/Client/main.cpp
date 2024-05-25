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
	//CClient* Client = new CClient();
	// initialise WSA
	InitWSA();

	// unique pointer so it is automatically deleted when it goes out of scope.
	std::unique_ptr<CClient> Client = std::make_unique<CClient>();

	if (Client->Setup())
	{
		Client->SendLoop();
	}

	WSACleanup();
	return 0;
}
