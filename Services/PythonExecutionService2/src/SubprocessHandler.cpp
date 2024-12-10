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
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_clientHandle) {
		if (m_healthCheckThread) {
			OT_LOG_E("Health check thread already exists");
			delete m_healthCheckThread;
			m_healthCheckThread = nullptr;
		}

		// Create start args
		std::string subprocessPath = this->findSubprocessPath();
		std::string commandLine = "\"" + subprocessPath + "\" \"" + _serverName + "\"";

		// Run sub
		ot::app::RunResult result = ot::app::runApplication(subprocessPath + m_executableName, commandLine, m_clientHandle, false, Timeouts::connectionTimeout);
		if (result != ot::app::OK) {
			m_clientHandle = nullptr;
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
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (!ot::app::isApplicationRunning(m_clientHandle)) {
				m_performHealthCheck = false;
				m_clientHandle = nullptr;
				OT_LOG_E("Client process stopped working");
			}
		}
	}
}
