// @otlicense
// File: ToolMenuManager.h
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
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtWidgets/qmenu.h>

class QAction;

class ToolMenuManager : public QMenu {
	Q_OBJECT
	OT_DECL_NOCOPY(ToolMenuManager)
public:
	ToolMenuManager(const QString& _toolName);
	virtual ~ToolMenuManager();

	QString toolName(void) const { return m_toolName; };

	QAction* runAction(void) { return m_run; };
	QAction* autorunAction(void) { return m_autorun; };

Q_SIGNALS:
	void runRequested(void);
	void autorunChanged(bool _isEnabled);

private Q_SLOTS:
	void slotRun(void);
	void slotAutorun(bool _checked);

private:
	QString m_toolName;
	QAction* m_run;
	QAction* m_autorun;

	ToolMenuManager() = delete;
};
