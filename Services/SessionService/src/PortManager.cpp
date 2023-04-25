
#include "PortManager.h"

#include <Windows.h>
#include <cassert>


PortManager::PortManager() :
	startingPortNumber(1024)
{

}

PortManager::~PortManager()
{

}

void PortManager::setIPAddress(const std::string &ip)
{
	ipAddress = ip;
}

void PortManager::setStartingPortNumber(unsigned long long startPortNum)
{
	startingPortNumber = startPortNum;
}

void PortManager::setPortInUse(unsigned long long portNum, bool inUse)
{
	portsInUse[portNum] = inUse;
}

bool PortManager::isPortInUse(unsigned long long portNum)
{
	if (portsInUse.count(portNum) == 0) return false;
	return portsInUse[portNum];
}

unsigned long long PortManager::determineAndBlockAvailablePort(void)
{
	assert(!ipAddress.empty());

	unsigned long long portNum = determineAvailablePort();
	setPortInUse(portNum, true);

	return portNum;
}

unsigned long long PortManager::determineAvailablePort(void)
{
	assert(!ipAddress.empty());

	// We start from the starting port number and increase the count until we have found an available port

	unsigned long long portNum = startingPortNumber;

	while (!portIsAvailable(portNum))
	{
		portNum++;

		if (portNum > 49151)
		{
			// We are exceeding the range of available ports of an application. This should normally not happen
			assert(0);
			return 0;
		}
	}

	return portNum;
}

bool PortManager::portIsAvailable(unsigned long long portNum)
{
	// First check whether we already know that the port is not available
	if (isPortInUse(portNum)) return false;

	// Now we did not occupy this port by ourselves, but anyother application might have done this. 
	// Therefore we check whether the port is already blocked.

	return true;

	/*
	bool portBusy = checkPortTCP((short int) portNum, ipAddress.c_str());

	if (portBusy)
	{
		// We would have expected this port to be available. Some other application seems to use it, so we will not
		// touch it anymore
		portsInUse[portNum] = true;
	}

	return !portBusy;
	*/
}

//bool PortManager::checkPortTCP(short int dwPort, const char *ipAddressStr)
//{
//	WSADATA wsaData;
//	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (iResult != NO_ERROR) {
////		wprintf(L"WSAStartup function failed with error: %d\n", iResult);
//		return false;
//	}
//
//	struct sockaddr_in client;
//	int sock;
//	client.sin_family = AF_INET;
//	client.sin_port = htons(dwPort);
//	client.sin_addr.s_addr = inet_addr(ipAddressStr);
//	sock = (int)socket(AF_INET, SOCK_STREAM, 0);
//	if (sock == INVALID_SOCKET) {
////		wprintf(L"ERROR: Socket function failed with error: %ld\n", WSAGetLastError());
//		WSACleanup();
//		return false;
//	}
//
//	printf("INFO: Checking Port : %s:%d\n", ipAddressStr, dwPort);
//	int result = connect(sock, (struct sockaddr *) &client, sizeof(client));
//	if (result == SOCKET_ERROR) {
////		printf("ERROR: %s", WSAGetLastError());
//		WSACleanup();
//		return false;
//	}
//	else
//	{
//		WSACleanup();
//		closesocket(sock);
//		return true;
//	}
//}