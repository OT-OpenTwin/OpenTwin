// Service header
#include "Timeouts.h"
#include "SubprocessHandler.h"

// OpenTwin header
#include "OTSystem/Application.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/Logger.h"

// std header
#include <thread>

SubprocessHandler::SubprocessHandler(SubprocessManager* _manager)
	: m_manager(_manager), m_process(nullptr), m_performHealthCheck(false), m_healthCheckThread(nullptr)
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
	if (!m_process) {
		if (m_healthCheckThread) {
			OT_LOG_E("Health check thread already exists");
			delete m_healthCheckThread;
			m_healthCheckThread = nullptr;
		}

		// Create start args
		std::string subprocessPath = this->findSubprocessPath();
		std::string commandLine = "\"" + _serverName + "\"";

		// Run sub
		ot::app::RunResult result = ot::app::runApplication(this->findSubprocessPath() + m_executableName, commandLine, m_process, true, Timeouts::connectionTimeout);
		if (result != ot::app::OK) {
			m_process = nullptr;
			OT_LOG_E("Failed to start subprocess");
			return false;
		}

		// Start health check
		m_performHealthCheck = true;
		m_healthCheckThread = new std::thread(&SubprocessHandler::healthCheckWorker, this);
	}
	
	return true;
}

std::string SubprocessHandler::findSubprocessPath(void) const {
#ifdef _DEBUG
	const char* otRootFolder = ot::os::getEnvironmentVariable("OPENTWIN_DEV_ROOT");
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

void SubprocessHandler::healthCheckWorker(void) {
	while (m_performHealthCheck) {

	}
}
