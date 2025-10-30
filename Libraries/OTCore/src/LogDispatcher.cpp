// @otlicense
// File: LogDispatcher.cpp
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

// Open Twin header
#include "OTSystem/OperatingSystem.h"
#include "OTCore/String.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/LogNotifierStdCout.h"
#include "OTCore/LogNotifierFileWriter.h"

// std header
#include <thread>
#include <iostream>
#include <exception>

#define OT_LOGGER_USE_MUTEX false

#if OT_LOGGER_USE_MUTEX==true
#include <mutex>
namespace ot {
	namespace intern {
		static std::atomic_bool g_logInitialized(false);
		static std::mutex g_logMutex;
	}
}
#endif

ot::LogDispatcher& ot::LogDispatcher::instance(void) {
	static LogDispatcher g_instance;
	return g_instance;
}

ot::LogDispatcher& ot::LogDispatcher::initialize(const std::string& _serviceName, bool _addCoutReceiver) {
	ot::LogDispatcher& dispatcher = ot::LogDispatcher::instance();

	// Setup name
	if (!_serviceName.empty()) {
		dispatcher.setServiceName(_serviceName);
	}

	// Add cout notifier if need
	if (_addCoutReceiver) {
		dispatcher.addReceiver(new LogNotifierStdCout);
	}

	// Add file notifier if needed
	if (String::toLower(OperatingSystem::getEnvironmentVariableString("OPEN_TWIN_FILE_LOGGING")) == "true") {
		dispatcher.addFileWriter();
	}

	return dispatcher;
}

void ot::LogDispatcher::addFileWriter(void) {
	LogDispatcher& dispatcher = LogDispatcher::instance();
	dispatcher.addReceiver(new LogNotifierFileWriter(dispatcher.m_serviceName + ".otlog"));
}

// ###########################################################################################################################################################################################################################################################################################################################

void ot::LogDispatcher::addReceiver(AbstractLogNotifier* _receiver) {
	m_messageReceiver.push_back(_receiver);
}

void ot::LogDispatcher::forgetReceiver(AbstractLogNotifier* _receiver) {
	auto it = std::find(m_messageReceiver.begin(), m_messageReceiver.end(), _receiver);
	if (it != m_messageReceiver.end()) {
		m_messageReceiver.erase(it);
	}
}

void ot::LogDispatcher::dispatch(const std::string& _text, const std::string& _functionName, const LogFlags& _logFlags) {
	this->dispatch(LogMessage(m_serviceName, _functionName, _text, _logFlags));
}

void ot::LogDispatcher::dispatch(const LogMessage& _message) {
#if OT_LOGGER_USE_MUTEX==true
	if (!intern::g_logInitialized) {
		intern::g_logInitialized = true;
		OT_LOG_W("Mutex in use for LogDispatcher!");
	}
	std::lock_guard<std::mutex> lock(intern::g_logMutex);
#endif

	if ((_message.getFlags() & m_logFlags) != _message.getFlags()) {
		return;
	}

	// Create Timestamp
	LogMessage msg(_message);
	msg.setCurrentTimeAsLocalSystemTime();
	msg.setUserName(m_userName);
	msg.setProjectName(m_projectName);

	for (auto r : m_messageReceiver) {
		try {
			r->log(msg);
		}
		catch (const std::exception& _e) {
			OTAssert(0, "Error occured while dispatching log message");
			std::cout << "Dispatch error: " << _e.what() << std::endl;
		}
	}
}

void ot::LogDispatcher::applyEnvFlag(const std::string& _str) {
	if (_str == "INFORMATION_LOG") m_logFlags |= ot::INFORMATION_LOG;
	else if (_str == "DETAILED_LOG") m_logFlags |= ot::DETAILED_LOG;
	else if (_str == "WARNING_LOG") m_logFlags |= ot::WARNING_LOG;
	else if (_str == "ERROR_LOG") m_logFlags |= ot::ERROR_LOG;
	else if (_str == "INBOUND_MESSAGE_LOG") m_logFlags |= ot::INBOUND_MESSAGE_LOG;
	else if (_str == "QUEUED_INBOUND_MESSAGE_LOG") m_logFlags |= ot::QUEUED_INBOUND_MESSAGE_LOG;
	else if (_str == "ONEWAY_TLS_INBOUND_MESSAGE_LOG") m_logFlags |= ot::ONEWAY_TLS_INBOUND_MESSAGE_LOG;
	else if (_str == "OUTGOING_MESSAGE_LOG") m_logFlags |= ot::OUTGOING_MESSAGE_LOG;
	else if (_str == "TEST_LOG") m_logFlags |= ot::TEST_LOG;
	else if (_str == "ALL_GENERAL_LOG_FLAGS") m_logFlags |= ot::ALL_GENERAL_LOG_FLAGS;
	else if (_str == "ALL_INCOMING_MESSAGE_LOG_FLAGS") m_logFlags |= ot::ALL_INCOMING_MESSAGE_LOG_FLAGS;
	else if (_str == "ALL_OUTGOING_MESSAGE_LOG_FLAGS") m_logFlags |= ot::ALL_OUTGOING_MESSAGE_LOG_FLAGS;
	else if (_str == "ALL_MESSAGE_LOG_FLAGS") m_logFlags |= ot::ALL_MESSAGE_LOG_FLAGS;
	else if (_str == "ALL_LOG_FLAGS") m_logFlags |= ot::ALL_LOG_FLAGS;
	else if (_str == "NO_LOG") {}
	else {
		OTAssert(0, "Unknown log flag");
	}
}

ot::LogDispatcher::LogDispatcher() : m_serviceName("NO SERVICE NAME PROVIDED"), m_logFlags(NO_LOG)
{
	// Get env
	char buffer[4096];
	size_t bufferLen;
	getenv_s(&bufferLen, buffer, sizeof(buffer) - 1, "OPEN_TWIN_LOGGING_MODE");
	std::string type(buffer);

	while (type.length() > 0 && type.at(0) == '\"') type.erase(type.begin());
	while (type.length() > 0 && type.at(type.length() - 1) == '\"') type.erase(type.begin() + (type.length() - 1));

	if (!type.empty()) {
		// Split
		auto ix = type.find('|');
		while (ix != std::string::npos) {
			std::string sub = type.substr(0, ix);
			type = type.substr(ix + 1);
			ix = type.find('|');

			if (!sub.empty()) applyEnvFlag(sub);
		}

		// Last entry
		if (!type.empty()) applyEnvFlag(type);
	}
	else {
		m_logFlags = WARNING_LOG | ERROR_LOG;
	}
}

ot::LogDispatcher::~LogDispatcher() {
	for (auto r : m_messageReceiver) {
		if (!r->getCustomDeleteLogNotifier()) {
			delete r;
		}
	}
	m_messageReceiver.clear();
}
