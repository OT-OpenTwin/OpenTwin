// Service header
#include "Timeouts.h"
#include "Application.h"
#include "SubprocessManager.h"
#include "SubprocessHandler.h"
#include "CommunicationHandler.h"

// Qt header
#include <QtCore/qcoreapplication.h>

// std header
#include <thread>

#ifdef _DEBUG
	//#define _SUBSERVICEDEBUG
#endif

SubprocessManager::SubprocessManager(Application* _app) 
	: m_app(_app), m_communicationHandler(nullptr), m_workerThread(nullptr)
{
	// Create subprocess handler
	m_subprocessHandler = new SubprocessHandler(this);

	// Start worker thread for Qt event loop
	m_workerThread = new std::thread(&SubprocessManager::worker, this, m_app->sessionID() + "_" OT_INFO_SERVICE_TYPE_PYRIT);
}

SubprocessManager::~SubprocessManager() {
	// Request qt event loop to quit if running
	QCoreApplication* qapp = QCoreApplication::instance();
	if (qapp) {
		QMetaObject::invokeMethod(qapp, &QCoreApplication::quit, Qt::QueuedConnection);
	}

	// Join worker if running
	if (m_workerThread) {
		m_workerThread->join();
		delete m_workerThread;
		m_workerThread = nullptr;
	}
}

void SubprocessManager::setModelUrl(const std::string& _url) {
	if (!this->ensureWorkerRunning()) {
		return;
	}
	
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_communicationHandler) {
		OT_LOG_E("Fatal Error: Communication handler not set");
		return;
	}
	return m_communicationHandler->setModelUrl(_url);
}

void SubprocessManager::setFrontendUrl(const std::string& _url) {
	if (!this->ensureWorkerRunning()) {
		return;
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_communicationHandler) {
		OT_LOG_E("Fatal Error: Communication handler not set");
		return;
	}
	return m_communicationHandler->setFrontendUrl(_url);
}

void SubprocessManager::setDataBaseInfo(const DataBaseInfo& _info) {
	if (!this->ensureWorkerRunning()) {
		return;
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_communicationHandler) {
		OT_LOG_E("Fatal Error: Communication handler not set");
		return;
	}
	return m_communicationHandler->setDataBaseInfo(_info);
}

bool SubprocessManager::sendRequest(const ot::JsonDocument& _document, std::string& _response) {
	if (!this->ensureWorkerRunning()) {
		return false;
	}

	if (!this->ensureSubprocessRunning()) {
		return false;
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	return m_communicationHandler->sendToClient(_document, _response);
}

bool SubprocessManager::sendSingleRequest(const ot::JsonDocument& _document, std::string& _response) {
	bool result = this->sendRequest(_document, _response);
	this->shutdownSubprocess();
	return result;
}

bool SubprocessManager::ensureSubprocessRunning(void) {

	std::string serverName;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		serverName = m_communicationHandler->getServerName();
	}
	if (serverName != "TestServerPython")
	{
		if (!m_subprocessHandler->ensureSubprocessRunning(serverName)) {
			OT_LOG_E("Failed to start subprocess");
			return false;
		}
	}
	else
	{
		assert(0); // Need to run PythonExecution now
	}

	return true;
}

void SubprocessManager::stopConnectionHandlingAfterCrash(void) {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_communicationHandler) {
		m_communicationHandler->cleanupAfterCrash();
	}
}

void SubprocessManager::shutdownSubprocess(void) {
	m_subprocessHandler->shutdownSubprocess();
}

bool SubprocessManager::ensureWorkerRunning(void) {
	const int tickTime = Timeouts::defaultTickTime;
	int timeout = Timeouts::connectionTimeout / tickTime;
	while (!isWorkerAlive()) {
		if (timeout--) {
			std::this_thread::sleep_for(std::chrono::milliseconds(tickTime));
		}
		else {
			OT_LOG_E("Worker thread failed to start");
			return false;
		}
	}

	return true;
}

bool SubprocessManager::isWorkerAlive(void) {
	std::lock_guard<std::mutex> lock(m_mutex);
	
	if (m_communicationHandler) {
		return m_communicationHandler->isListening();
	}
	else
	{
		return false;
	}
	
}

void SubprocessManager::worker(std::string _projectName) {
	try {
		// Create Qt application
		int argc = 0;
		QCoreApplication app(argc, nullptr);

		// Create communication handler
		{
			std::lock_guard<std::mutex> lock(m_mutex);
#ifdef _SUBSERVICEDEBUG
			m_communicationHandler = new CommunicationHandler(this, "TestServerPython");
#else
			m_communicationHandler = new CommunicationHandler(this, _projectName);
#endif		
		}

		// Run Qt event loop
		app.exec();
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS("Fatal Error: " + std::string(_e.what()));
	}
	catch (...) {
		OT_LOG_EA("Fatal Error: Unknown error");
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	OT_LOG_D("Worker shut down completed.");

	delete m_communicationHandler;
	m_communicationHandler = nullptr;

}
