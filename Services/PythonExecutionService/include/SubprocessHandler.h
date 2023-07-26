#pragma once
#include <string>
#include "openTwinSystem/SystemTypes.h"

class SubprocessHandler
{

public:
	SubprocessHandler();
	void Create(const std::string& urlThisProcess);
	bool Close();

private:
	const int _startPort = 7800;
	const int _maxPingAttempts = 3;
	std::string _pingCommand = "";

	std::string _launcherPath = "";
	std::string _subprocessPath = "";
	
	std::string _urlSubprocess = "";

	OT_PROCESS_HANDLE _subprocess = nullptr;

	void RunWithNextFreeURL(const std::string& urlThisService);
	
	bool CheckAlive(OT_PROCESS_HANDLE& handle);
	bool PingSubprocess();
};
