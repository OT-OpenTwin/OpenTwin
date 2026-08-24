// @otlicense
// File: StatusManager.h
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
#include <QtCore/qstring.h>
#include <QtWidgets/qstatusbar.h>	// Base class

// std header
#include <map>
#include <list>

class QTimer;
class QLabel;

class StatusManager : public QStatusBar {
	Q_OBJECT
public:
	StatusManager();
	virtual ~StatusManager();

	//! @brief Will set the provided text as information text to the statusbar
	void setInfo(const QString& _text);
	
	//! @brief Will set the provided text as error text to the statusbar
	void setErrorInfo(const QString& _text);

	void setCurrentTool(const QString& _toolName);

	void addTool(const QString& _toolName, const std::list<QWidget*>& _widgets);
	void removeTool(const QString& _toolName);

private Q_SLOTS:
	void slotResetErrorStatus(void);

private:
	bool						m_statusIsError;
	
	QString						m_statusText;

	QTimer *					m_timerErrorStatusReset;
	QLabel *					m_infoLabel;
	QLabel *					m_stretchLabel;

	QString						m_currentTool;
	std::map<QString, std::list<QWidget*>> m_toolWidgets;

};
