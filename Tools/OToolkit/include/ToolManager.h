// @otlicense
// File: ToolManager.h
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

// Toolkit header

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qobject.h>
#include <QtWidgets/qmenu.h>

// std header
#include <map>

class AppBase;
class MenuManager;
class StatusManager;
class ToolBarManager;
class ToolBarManager;
class SettingsManager;
class ToolViewManager;
class ToolRuntimeHandler;

namespace otoolkit { class Tool; };

class ToolManager : public QObject {
	Q_OBJECT
	OT_DECL_NOCOPY(ToolManager)
public:
	ToolManager(AppBase* _app);
	virtual ~ToolManager();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	//! @brief Stores the provided tool
	bool addTool(otoolkit::Tool* _tool);

	//! @brief Returns the tool with the specified name
	//! If no tool was found nullptr will be returned
	otoolkit::Tool* findTool(const QString& _toolName);

	//! @brief Cleans up and deletes the tool with the provided name
	void removeTool(const QString& _toolName);

	//! @brief Cleans up the provided tool
	//! The tool will not be deleted
	void removeTool(otoolkit::Tool* _tool);

	void clear(void);

	MenuManager* getMenuManager(void) { return m_menuManager; };
	StatusManager* getStatusManager(void) { return m_statusManager; };
	ToolBarManager* getToolBarManager(void) { return m_toolBarManager; };
	ToolViewManager* getToolViewManager(void) { return m_toolViewManager; };
	SettingsManager* getToolSettingsManager(void) { return m_settingsManager; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Runtime handling

	void runTool(const QString& _toolName);
	
	void stopAll(void);

	void stopTool(const QString& _toolName);

	void toolDataHasChanged(ToolRuntimeHandler* _handler);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotRunToolTriggered(void);
	void slotViewFocused(const QString& _viewName, const QString& _toolName);
	void slotViewFocusLost(const QString& _viewName, const QString& _toolName);
	void slotViewCloseRequested(const QString& _viewName, const QString& _toolName);
	void slotSettingsRequested(void);

private:
	void fwdRemoveTool(const QString& _toolName);


	std::map<QString, ToolRuntimeHandler*> m_tools;

	bool m_ignoreEvents;
	MenuManager* m_menuManager;
	StatusManager* m_statusManager;
	ToolBarManager* m_toolBarManager;
	ToolViewManager* m_toolViewManager;
	SettingsManager* m_settingsManager;

	ToolManager() = delete;
};
