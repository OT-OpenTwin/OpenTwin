#pragma once

#include <string>
#include <map>

class PortManager
{
public:
	PortManager();
	virtual ~PortManager();

	void setIPAddress(const std::string &ip);
	void setStartingPortNumber(unsigned long long startPortNum);

	void setPortInUse(unsigned long long portNum, bool inUse);
	bool isPortInUse(unsigned long long portNum);

	unsigned long long determineAvailablePort(void);
	unsigned long long determineAndBlockAvailablePort(void);

private:
	bool portIsAvailable(unsigned long long portNum);
	//bool checkPortTCP(short int dwPort, const char *ipAddressStr);

	std::string ipAddress;
	std::map<unsigned long long, bool> portsInUse;
	unsigned long long startingPortNumber;
};
