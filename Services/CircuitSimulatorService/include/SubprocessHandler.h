// @otlicense
// File: SubprocessHandler.h
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
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/ReturnMessage.h"


#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qprocess.h>
#include <QtCore/qdebug.h>
#include "QtCore/qobject.h"

#include <atomic>
#include <chrono>



class SubprocessHandler : public BusinessLogicHandler
{
	
public:
	SubprocessHandler(const std::string& serverName, int sessionID, int serviceID);
	~SubprocessHandler();

	bool RunSubprocess();
	bool startSubprocess();
	void stopSubprocess();

private:	
	const std::string m_executableName = "CircuitExecution.exe";
	std::string m_subprocessPath;
	
	std::atomic_bool m_isHealthy;
	std::atomic_bool m_initialisationPrepared = false;

	QProcess m_subProcess;
	std::string m_serverName;
	std::string FindSubprocessPath();
	void InitiateProcess();

	const int m_numberOfRetries = 3;
	const int m_timeoutSubprocessStart = 50000;//5 seconds

	const int m_heartBeat = m_timeoutSubprocessStart;
	
	void ProcessErrorOccured(std::string& message);
};