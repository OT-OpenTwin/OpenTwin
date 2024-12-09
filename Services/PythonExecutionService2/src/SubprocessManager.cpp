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

SubprocessManager::SubprocessManager(Application* _app) 
	: m_app(_app), m_communicationHandler(nullptr), m_workerThread(nullptr)
{
	// Create subprocess handler
	m_subprocessHandler = new SubprocessHandler(this);

	// Start worker thread for Qt event loop
	m_workerThread = new std::thread(&SubprocessManager::worker, this, m_app->sessionID() + "_" OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE);
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

bool SubprocessManager::sendRequest(const ot::JsonDocument& _document, std::string& _response) {
	if (!this->ensureSubprocessRunning()) {
		return false;
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	return m_communicationHandler->sendToClient(_document, _response);
}

bool SubprocessManager::ensureSubprocessRunning(void) {
	const int tickTime = 10;
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
	bool ret = false;
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_communicationHandler) {
		ret = m_communicationHandler->isListening();
	}
	return ret;
}

void SubprocessManager::worker(std::string _projectName) {
	try {
		// Create Qt application
		int argc = 0;
		QCoreApplication app(argc, nullptr);

		// Create communication handler
		std::unique_lock<std::mutex> lock(m_mutex);
		m_communicationHandler = new CommunicationHandler(this, _projectName);
		lock.unlock();

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
