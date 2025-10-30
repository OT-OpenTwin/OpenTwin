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

// Qt header
#include <QtCore/qobject.h>

// std header
#include <string>

class QLocalSocket;

class CommunicationHandler : public QObject {
	Q_OBJECT
public:
	CommunicationHandler();
	virtual ~CommunicationHandler();

	void setServerName(const std::string& _serverName) { m_serverName = _serverName; };
	const std::string& getServerName(void) const { return m_serverName; };

	bool ensureConnectionToServer(void);
	bool writeToServer(const std::string& _message);

Q_SIGNALS:
	void disconnected(void);

private Q_SLOTS:
	void slotDataReceived(void);
	void slotProcessMessage(std::string _message);
	void slotDisconnected(void);

private:
	void processNextEvent(void);

	std::string m_serverName;
	QLocalSocket* m_serverSocket;

};