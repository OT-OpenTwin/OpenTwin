// @otlicense
// File: AppBase.h
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

#pragma once

// Service header
#include "FileManager.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/AbstractLogNotifier.h"
#include "OTCommunication/ActionHandler.h"

// std header
#include <list>
#include <mutex>
#include <string>
#include <thread>

class AppBase : public ot::ServiceBase, public ot::AbstractLogNotifier, public ot::ActionHandler {
	OT_DECL_NOCOPY(AppBase)
public:
	static AppBase& instance(void);

	//! @brief Called when the a log message was created.
	virtual void log(const ot::LogMessage& _message) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	void handleLog(ot::JsonDocument& _jsonDocument);
	std::string handleRegister(ot::JsonDocument& _jsonDocument);
	void handleDeregister(ot::JsonDocument& _jsonDocument);
	void handleClear();
	std::string handleGetDebugInfo();
	void handleSetGlobalLogFlags(ot::JsonDocument& _jsonDocument);
	void handleSetCacheSize(ot::JsonDocument& _jsonDocument);
	ot::ReturnMessage handleGetAllLogs();
	ot::ReturnMessage handleGetUserLogs(ot::JsonDocument& _jsonDocument);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	void updateBufferSizeFromLogFlags(const ot::LogFlags& _flags);

private:
	void appendLogMessage(ot::LogMessage&& _message);

	void workerNotify();

	//! @warning It is assumed that m_mutex is already locked.
	void removeReceiver(const std::string& _receiver);

	//! @warning It is assumed that m_mutex is already locked.
	void resizeBuffer(void);

	std::mutex                  m_newMessageMutex;
	std::list<ot::LogMessage>   m_newMessages;

	std::mutex					m_mutex;
	std::thread*                m_notifyThread;
	std::atomic_bool            m_notifyThreadRunning;

	size_t						m_bufferSize;
	std::list<ot::LogMessage>	m_messages;
	size_t						m_count;

	std::list<std::string>		m_receiver;

	FileManager m_fileManager;

	AppBase();
	virtual ~AppBase();
};