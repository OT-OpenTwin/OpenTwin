// @otlicense
// File: MenuManager.cpp
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

// Toolkit header
#include "MenuManager.h"
#include "ToolMenuManager.h"

// ToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTWidgets/GlobalWidgetViewManager.h"

// Qt header
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qaction.h>

#define MENU_LOG(___msg) OTOOLKIT_LOG("Menu", ___msg)
#define MENU_LOGW(___msg) OTOOLKIT_LOGW("Menu", ___msg)
#define MENU_LOGE(___msg) OTOOLKIT_LOGE("Menu", ___msg)

MenuManager::MenuManager() 
	: m_externalToolsMenu(nullptr)
{
	// Create root menus
	m_fileMenu = this->addMenu("File");
	this->addAction(ot::GlobalWidgetViewManager::instance().getDockToggleAction());
	m_toolsMenu = this->addMenu("Tools");

	// Settings
	QAction* settingsAction = m_fileMenu->addAction("Settings");
	settingsAction->setIcon(QIcon(":/images/Settings.png"));
	this->connect(settingsAction, &QAction::triggered, this, &MenuManager::slotSettings);

	m_fileMenu->addSeparator();

	// Exit
	QAction* exitAction = m_fileMenu->addAction("Exit");
	exitAction->setIcon(QIcon(":/images/Exit.png"));
	this->connect(exitAction, &QAction::triggered, this, &MenuManager::slotExit);
}

MenuManager::~MenuManager() {

}

ToolMenuManager* MenuManager::addToolMenu(const QString& _toolName, bool _isExternal) {
	// Check if menu already exists
	ToolMenuManager* menu = this->toolMenu(_toolName);
	if (menu) {
		MENU_LOGW("Tool menu already exists { \"ToolName\": \"" + _toolName + "\" }");
		return menu;
	}

	menu = new ToolMenuManager(_toolName);
	m_toolMenus.insert_or_assign(_toolName, menu);

	if (_isExternal) {
		if (!m_externalToolsMenu) {
			m_externalToolsMenu = m_toolsMenu->addMenu("External");
		}
		m_externalToolsMenu->addMenu(menu);
	}
	else {
		m_toolsMenu->addMenu(menu);
	}
		
	return menu;
}

ToolMenuManager* MenuManager::toolMenu(const QString& _toolName) {
	auto it = m_toolMenus.find(_toolName);
	if (it == m_toolMenus.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

void MenuManager::slotExit(void) {
	Q_EMIT exitRequested();
}

void MenuManager::slotSettings(void) {
	Q_EMIT settingsRequested();
}
