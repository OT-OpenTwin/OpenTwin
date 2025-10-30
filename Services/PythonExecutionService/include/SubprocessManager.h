// @otlicense
// File: SubprocessManager.h
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
#include "DataBaseInfo.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <mutex>

class Application;
class SubprocessHandler;
class CommunicationHandler;

namespace std { class thread; }

class SubprocessManager {
	OT_DECL_NOCOPY(SubprocessManager)
	OT_DECL_NODEFAULT(SubprocessManager)
public:
	SubprocessManager(Application* _app);
	~SubprocessManager();
	
	void setModelUrl(const std::string& _url);
	void setFrontendUrl(const std::string& _url);
	void setDataBaseInfo(const DataBaseInfo& _info);

	//! @brief Sends the request to the Subprocess.
	//! Will start the subprocess if needed.
	bool sendRequest(const ot::JsonDocument& _document, std::string& _response);

	//! @brief Sends the request to the Subprocess.
	//! Will start the subprocess if needed.
	//! Will shutdown the subprocess after finished.
	bool sendSingleRequest(const ot::JsonDocument& _document, std::string& _response);

	bool ensureSubprocessRunning(void);

	//! @brief Stops the connection handler.
	void stopConnectionHandlingAfterCrash(void);

	bool isConnected();

private:
	void shutdownSubprocess(void);

	//! @brief Runs the subservice if needed and checks connection with a ping.
	//! @return Return false if the connection could not be established.
	bool ensureWorkerRunning(void);

	//! @brief Returns true if the worker has started successfully.
	//! Starting the working will only start the event loop and communication handler.
	bool isWorkerAlive(void);

	//! @brief Event loop worker.
	void worker(std::string _projectName);

	std::thread* m_workerThread;
	std::mutex m_mutex;

	Application* m_app;

	SubprocessHandler* m_subprocessHandler;
	CommunicationHandler* m_communicationHandler;
};