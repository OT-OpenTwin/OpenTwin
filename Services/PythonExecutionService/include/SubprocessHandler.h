#pragma once
#include <string>
#include "openTwinSystem/SystemTypes.h"
#include "OpenTwinCore/rJSON.h"

#include <condition_variable>
#include <atomic>
#include <chrono>

using namespace std::chrono_literals;
class SubprocessHandler
{

public:
	SubprocessHandler(const std::string& urlThisService);
	~SubprocessHandler();
	SubprocessHandler& operator=(const SubprocessHandler& other) = delete;
	SubprocessHandler& operator=(SubprocessHandler&& other) = delete;
	SubprocessHandler(const SubprocessHandler& other) =  delete;
	SubprocessHandler(SubprocessHandler&& other) =  delete;

	void setReceivedInitializationRequest();
	bool getReceivedInitializationRequest() { return _receivedInitializationRequest; };
	static int getStartPort() { return _startPort; }
	void setSubprocessURL(const std::string& urlSubprocess) { _urlSubprocess = urlSubprocess; };
	
	void SendExecutionOrder(OT_rJSON_doc& scriptsAndParameter);
	void Create(const std::string& urlThisProcess);
	bool Close();

private:
	std::string _pingCommand = "";

	std::string _launcherPath = "";
	std::string _subprocessPath = "";
	
	std::string _urlSubprocess = "";
	std::string _urlThisProcess = "";

	OT_PROCESS_HANDLE _subprocess = nullptr;

	static const int _startPort = 7800;
	std::chrono::seconds _processCreationTimeOut = 2s; // sec
	const int _maxPingAttempts = 3;

	std::condition_variable _waitForInitializationRequest;
	std::atomic<bool> _receivedInitializationRequest = false;

	void RunWithNextFreeURL(const std::string& urlThisService);
	
	bool CheckAlive(OT_PROCESS_HANDLE& handle);
	bool PingSubprocess();
	bool CloseProcess(const std::string& url);
};
