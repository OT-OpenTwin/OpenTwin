// @otlicense
// File: LogServiceDebugInfo.h
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
#include "OTCore/ServiceDebugInformation.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qsettings.h>

class QTableWidgetItem;
namespace ot { class Table; }

class LogServiceDebugInfo : public QObject {
	Q_OBJECT
public:
	LogServiceDebugInfo();
	virtual ~LogServiceDebugInfo();
	
	QWidget* getRootWidget() const { return m_root; };

	void restoreSettings(QSettings& _settings);
	void saveSettings(QSettings& _settings);
	
	void appendServiceDebugInfo(const ot::ServiceDebugInformation& _info);

private Q_SLOTS:
	void slotClear();
	void slotItemDoubleClicked(QTableWidgetItem* _item);

private:
	QWidget* m_root;

	ot::Table* m_table;
};