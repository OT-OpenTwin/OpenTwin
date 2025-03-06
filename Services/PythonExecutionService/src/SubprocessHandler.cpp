// Service header
#include "Timeouts.h"
#include "SubprocessHandler.h"
#include "SubprocessManager.h"

// OpenTwin header
#include "OTSystem/SystemProcess.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/Logger.h"

// std header
#include <thread>

SubprocessHandler::SubprocessHandler(SubprocessManager* _manager)
	: m_manager(_manager), m_clientHandle(nullptr), m_performHealthCheck(false), m_healthCheckThread(nullptr)
{
	OTAssertNullptr(m_manager);
}

SubprocessHandler::~SubprocessHandler() {
	m_performHealthCheck = false;
	if (m_healthCheckThread) {
		m_healthCheckThread->join();
		delete m_healthCheckThread;
		m_healthCheckThread = nullptr;
	}
}

bool SubprocessHandler::ensureSubprocessRunning(const std::string& _serverName) {
	if (m_clientHandle) {
		return true;
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_healthCheckThread) {
		if (m_performHealthCheck) {
			OT_LOG_E("Health check thread already exists");
		}
		m_performHealthCheck = false;
		m_healthCheckThread->join();
		delete m_healthCheckThread;
		m_healthCheckThread = nullptr;
	}

	// Create start args
	std::string subprocessPath = this->findSubprocessPath();
	std::string commandLine = "\"" + subprocessPath + "\" \"" + _serverName + "\"";

	// Run sub
	ot::SystemProcess::RunResult result = ot::SystemProcess::runApplication(subprocessPath + m_executableName, commandLine, m_clientHandle);
	if (result.isOk()) {
		m_clientHandle = nullptr;
		OT_LOG_E("Failed to start subprocess");
		return false;
	}

	// Start health check
	m_performHealthCheck = true;
	m_healthCheckThread = new std::thread(&SubprocessHandler::healthCheckWorker, this);

	return true;
}

std::string SubprocessHandler::findSubprocessPath(void) const {
#ifdef _DEBUG
	const char* otRootFolder = ot::OperatingSystem::getEnvironmentVariable("OPENTWIN_DEV_ROOT");
	if (otRootFolder) {
		std::string result = std::string(otRootFolder) + "\\Deployment\\";
		delete[] otRootFolder;
		otRootFolder = nullptr;
		return result;
	}
	else {
		OT_LOG_E("OPENTWIN_DEV_ROOT environment not set");
		return ".\\";
	}
#else
	return ".\\";
#endif // DEBUG

}

void SubprocessHandler::shutdownSubprocess(void) {
	if (!m_clientHandle) {
		return;
	}

	// Stop health check
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_performHealthCheck = false;
	}
	if (m_healthCheckThread) {
		m_healthCheckThread->join();
		delete m_healthCheckThread;
		m_healthCheckThread = nullptr;
	}

	// Terminate the subprocess
	if (TerminateProcess(m_clientHandle, 0)) {
		CloseHandle(m_clientHandle);
		m_clientHandle = nullptr;
		OT_LOG_D("Subprocess terminated");
	}
	else {
		OT_LOG_EA("Failed to terminate subproces");
	}
}

void SubprocessHandler::healthCheckWorker(void) {
	while (m_performHealthCheck) {
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (!ot::SystemProcess::isApplicationRunning(m_clientHandle)) {
				m_performHealthCheck = false;
				
				CloseHandle(m_clientHandle);
				m_clientHandle = nullptr;

				OT_LOG_E("Client process stopped working");

				if (m_manager) {
					m_manager->stopConnectionHandlingAfterCrash();
				}
			}
		}
	}
}

