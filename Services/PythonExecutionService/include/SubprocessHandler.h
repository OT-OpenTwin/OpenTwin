#pragma once
#include <string>
#include "openTwinSystem/SystemTypes.h"
#include <condition_variable>
#include <atomic>

class SubprocessHandler
{

public:
	SubprocessHandler();
	
	SubprocessHandler& operator=(const SubprocessHandler& other) = delete;
	SubprocessHandler& operator=(SubprocessHandler&& other) = delete;
	SubprocessHandler(const SubprocessHandler& other) =  delete;
	SubprocessHandler(SubprocessHandler&& other) =  delete;

	void Create(const std::string& urlThisProcess);

	static int getStartPort() { return _startPort; }
	void setReceivedInitializationRequest();
	bool getReceivedInitializationRequest() { return _receivedInitializationRequestAlready; };
	bool Close();
private:
	static const int _startPort = 7800;
	const int _maxTimeOut = 3;

	std::condition_variable _receivedInitializationRequest;
	std::atomic<bool> _receivedInitializationRequestAlready = false;


	std::string _pingCommand = "";

	std::string _launcherPath = "";
	std::string _subprocessPath = "";
	
	std::string _urlSubprocess = "";

	OT_PROCESS_HANDLE _subprocess = nullptr;

	void RunWithNextFreeURL(const std::string& urlThisService);
	
	bool CheckAlive(OT_PROCESS_HANDLE& handle);
	bool PingSubprocess();
	bool CloseProcess(const std::string& url);
};
