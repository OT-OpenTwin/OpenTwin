// @otlicense
// File: ConnectToLoggerDialog.h
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtCore/qlist.h>

// std header
#include <list>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class ConnectToLoggerDialog : public ot::Dialog {
	Q_OBJECT
public:
	ConnectToLoggerDialog();
	virtual ~ConnectToLoggerDialog();

	//! @brief Moves all messages from the internal message buffer to the provided list.
	//! @param _messages List that will receive the messages. The list will be cleared before adding the new messages.
	void grabMessageBuffer(std::list<ot::LogMessage>& _messages) { _messages = std::move(m_messageBuffer); m_messageBuffer.clear(); };

	QString loggerServiceUrl(void) const;

	void queueConnectRequest(void);
	void queueRecenterRequest(void);

protected:
	virtual bool mayCloseDialogWindow(void) override;

private Q_SLOTS:
	void slotConnect(void);
	void slotPingFail(void);
	void slotRegisterFail(void);
	void slotDone(void);
	void slotRecenter(void);
	void slotEnable(void);

private:
	void worker(QString _url);
	void stopWorker(void);

	std::atomic_bool			m_workerRunning;
	std::atomic_bool			m_stopWorker;

	QVBoxLayout *				m_centralLayout;
	QGridLayout *				m_inputLayout;
	QHBoxLayout *				m_buttonLayout;

	QLabel *					m_urlL;
	QLineEdit *					m_url;

	QPushButton *				m_btnCancel;
	QPushButton *				m_btnConnect;

	std::list<ot::LogMessage>	m_messageBuffer;
};
