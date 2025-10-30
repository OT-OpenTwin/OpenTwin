// @otlicense
// File: MenuManager.h
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
#include <QtWidgets/qmenubar.h>

// std header
#include <map>

class ToolMenuManager;

class QMenu;

class MenuManager : public QMenuBar {
	Q_OBJECT
	OT_DECL_NOCOPY(MenuManager)
public:
	MenuManager();
	virtual ~MenuManager();

	QMenu* getFileMenu(void) { return m_fileMenu; };
	QMenu* getToolsMenu(void) { return m_toolsMenu; };

	ToolMenuManager* addToolMenu(const QString& _toolName, bool _isExternal);
	ToolMenuManager* toolMenu(const QString& _toolName);

Q_SIGNALS:
	void exitRequested(void);
	void settingsRequested(void);

private Q_SLOTS:
	void slotExit(void);
	void slotSettings(void);
	
private:
	QMenu* m_fileMenu;
	QMenu* m_toolsMenu;
	QMenu* m_externalToolsMenu;

	std::map<QString, ToolMenuManager*> m_toolMenus;

};
