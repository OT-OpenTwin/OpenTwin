// @otlicense
// File: ConnectionManager.h
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
#include "QtNetwork/qlocalsocket.h"
#include "QtCore/qobject.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ServiceLogNotifier.h"

class NGSpice;

class ConnectionManager : public QObject {
	Q_OBJECT
public:

	enum RequestType {
		SendResults,
		Message,
		Error
	};
	static QString toString(RequestType _type);

	ConnectionManager(QObject* parent = (QObject*)nullptr);
	~ConnectionManager();

	void connectToCircuitSimulatorService(const QString& serverName);
	void sendBackResults(std::map<std::string, std::vector<double>>& _results);
public slots:
	void sendMessage(std::string messageType,std::string message);
private slots:
	void receiveResponse();
	void handleError(QLocalSocket::LocalSocketError error);
	void handleDisconnected();
	void sendHealthCheck();
	void handleConnected();

private:
	QLocalSocket* m_socket;
	QTimer* healthCheckTimer;
	bool waitForHealthcheck;
	void handleActionType(QString _actionType, QJsonArray _data);
	void handleRunSimulation( std::list<std::string> _netlist);
	void sendJson(QByteArray _data);
	

	

};