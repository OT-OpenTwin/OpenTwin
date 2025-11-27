// @otlicense
// File: SubprocessManager.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// Service header
#include "Timeouts.h"
#include "Application.h"
#include "SubprocessManager.h"
#include "SubprocessHandler.h"
#include "CommunicationHandler.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTCommunication/ActionTypes.h"

// Qt header
#include <QtCore/qcoreapplication.h>

// std header
#include <thread>

SubprocessManager::SubprocessManager(Application* _app) 
	: m_app(_app), m_communicationHandler(nullptr), m_workerThread(nullptr)
{
	// Create subprocess handler
	m_subprocessHandler = new SubprocessHandler(this);

	// Start worker thread for Qt event loopB
	m_workerThread = new std::thread(&SubprocessManager::worker, this, m_app->getSessionID());
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

void SubprocessManager::setManifestUID(const std::string& _manifestUID)
{
	assert(m_communicationHandler != nullptr);
	m_communicationHandler->setEnvironmentID(_manifestUID);
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
#ifndef _DEBUG
	std::string serverName;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		serverName = m_communicationHandler->getServerName();
	}
	if (!m_subprocessHandler->ensureSubprocessRunning(serverName)) {
		OT_LOG_E("Failed to start subprocess");
		return false;
	}
#endif
	return true;
}

void SubprocessManager::stopConnectionHandlingAfterCrash(void) {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_communicationHandler) {
		m_communicationHandler->cleanupAfterCrash();
	}
}

bool SubprocessManager::isConnected() {
	if (m_communicationHandler) {
		return !m_communicationHandler->isDisconnected();
	}
	else {
		return false;
	}
}

void SubprocessManager::shutdownSubprocess(void) {
	m_subprocessHandler->shutdownSubprocess();
}

//! @brief Waits for currently 30 sec and checks in an interval of 10 msec, if the qt server is listening
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
#ifdef _DEBUG
			m_communicationHandler = new CommunicationHandler(this, "TestServerPython");
#else
			// Encode project name to base64 to avoid issues with special characters
			const std::string hexString = ot::String::toBase64Url(_projectName);

			m_communicationHandler = new CommunicationHandler(this, OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE "_" + hexString);
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
