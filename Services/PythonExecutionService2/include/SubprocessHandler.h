#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTSystem/SystemTypes.h"

// std header
#include <mutex>
#include <string>

class SubprocessManager;
namespace std { class thread; };

class SubprocessHandler {
	OT_DECL_NOCOPY(SubprocessHandler)
	OT_DECL_NODEFAULT(SubprocessHandler)
public:
	SubprocessHandler(SubprocessManager* _manager);
	~SubprocessHandler();

	bool ensureSubprocessRunning(const std::string& _serverName);

private:
	std::string findSubprocessPath(void) const;
	void healthCheckWorker(void);

	std::mutex m_mutex;

	bool m_performHealthCheck;
	SubprocessManager* m_manager;
	const std::string m_executableName = "PythonExecution.exe";
	
	std::thread* m_healthCheckThread;
	OT_PROCESS_HANDLE m_clientHandle;
};