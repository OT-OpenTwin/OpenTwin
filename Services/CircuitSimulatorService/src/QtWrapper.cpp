
//Service Header
#include "QtWrapper.h"
#include "ConnectionManager.h"
//std Header
#include <thread>

//Qt Header
#include <QtCore/qcoreapplication.h>
#include <QtNetwork/qlocalsocket.h>


QtWrapper::QtWrapper() {
	m_application = nullptr;
	m_connectionManager = nullptr;
}

void QtWrapper::run(const std::string& _serverName) {
	std::thread workerThread(&QtWrapper::worker, this, _serverName);
	workerThread.detach();
}

void QtWrapper::worker(std::string _serverName) {
	int argc = 0;
	m_application = new QCoreApplication(argc, nullptr);
	m_connectionManager = new ConnectionManager;
	m_connectionManager->startListen(_serverName);
	

	m_application->exec();

	delete m_connectionManager;
	m_connectionManager = nullptr;
}

