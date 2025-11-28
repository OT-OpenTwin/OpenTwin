// @otlicense
// File: CommunicationHandler.h
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
#include "OTSystem/Flags.h"
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtNetwork/qlocalserver.h>
#include <mutex>

class SubprocessManager;

class CommunicationHandler : public QLocalServer {
	Q_OBJECT
	OT_DECL_NOCOPY(CommunicationHandler)
	OT_DECL_NODEFAULT(CommunicationHandler)
public:
	CommunicationHandler(SubprocessManager* _manager, const std::string& _serverName);
	~CommunicationHandler();

	bool sendToClient(const ot::JsonDocument& _document, std::string& _response);

	void setModelUrl(const std::string& _url) { m_modelUrl = _url; m_modelUrlSet = false; };
	void setFrontendUrl(const std::string& _url) { m_frontendUrl = _url; m_frontendUrlSet = false; };
	void setDataBaseInfo(const DataBaseInfo& _info) { m_databaseInfo = _info; m_databaseInfoSet = false; };

	const std::string& getServerName(void) const { return m_serverName; };

	bool sendConfigToClient(void);

	void cleanupAfterCrash(void);

	bool isDisconnected() const { return m_clientState == ClientState::Disconnected; };

	void setManifestUID(ot::UID _manifestUID)
	{
		m_manifestUID = _manifestUID;
	}

private Q_SLOTS:
	void slotNewConnection(void);
	void slotMessageReceived(void);
	void slotClientDisconnected(void);
	void slotProcessMessage(std::string _message);

private:
	enum class ClientState {
		Disconnected,
		WaitForPing,
		Ready,
		WaitForResponse,
		ReponseReceived
	};

	void processNextEvent(void);

	bool sendServiceInfoToClient(void);
	bool sendModelConfigToClient(void);
	bool sendFrontendConfigToClient(void);
	bool sendDataBaseConfigToClient(void);

	ClientState getClientState();
	void setClientState(ClientState _clientState);

	//! @brief Waits the default timeout (30 sec) and checks every 10 msec if the client state has changed
	bool waitForClient(void);
	
	//! @brief Danger? Waits infinitly for a response
	bool sendToClient(const QByteArray& _data, bool _expectResponse, std::string& _response);

	SubprocessManager* m_manager;
	std::string m_serverName;

	QLocalSocket* m_client;
	ClientState m_clientState; //State is accessed by two threads, use the threadsafe getter and setter
	std::mutex m_clientStateMutex;

	bool m_isInitializingClient;

	bool m_serviceAndSessionInfoSet;

	std::string m_frontendUrl;
	bool m_frontendUrlSet;

	std::string m_modelUrl;
	bool m_modelUrlSet;

	DataBaseInfo m_databaseInfo;
	bool m_databaseInfoSet;

	std::string m_response;
	ot::UID m_manifestUID = ot::invalidUID;
};