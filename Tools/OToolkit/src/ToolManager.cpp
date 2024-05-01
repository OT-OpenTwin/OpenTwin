//! @file ToolManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "ToolManager.h"
#include "MenuManager.h"
#include "StatusManager.h"
#include "ToolBarManager.h"
#include "ToolMenuManager.h"
#include "ToolRuntimeHandler.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/WidgetViewManager.h"

ToolManager::ToolManager(QMainWindow* _mainWindow)
	: m_ignoreEvents(false)
{
	m_menuManager = new MenuManager;
	m_statusManager = new StatusManager;
	m_toolBarManager = new ToolBarManager(_mainWindow);

	//this->connect(m_tabManager, &TabManager::currentToolChanged, this, &ToolManager::currentToolChanged);
}

ToolManager::~ToolManager() {
	this->clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

bool ToolManager::addTool(otoolkit::Tool* _tool) {
	otoolkit::Tool* t = this->findTool(_tool->toolName());
	if (t != nullptr) {
		OT_LOG_W("Tool already exists: \"" + _tool->toolName().toStdString() + "\"");
		return false;
	}
	m_tools.insert_or_assign(_tool->toolName(), new ToolRuntimeHandler(_tool));

	// Create menu and connect the run signal
	ToolMenuManager* tmm = m_menuManager->addToolMenu(_tool->toolName());
	this->connect(tmm, &ToolMenuManager::runRequested, this, &ToolManager::runToolTriggered);

	return true;
}

otoolkit::Tool* ToolManager::findTool(const QString& _toolName) {
	auto it = m_tools.find(_toolName);
	if (it == m_tools.end()) {
		return nullptr;
	}
	return it->second->tool();
}

void ToolManager::removeTool(const QString& _toolName) {
	otoolkit::Tool* tool = this->findTool(_toolName);
	if (tool == nullptr) {
		OT_LOG_E("Tool not found { \"Name\": \"" + _toolName.toStdString() + "\" }");
		return;
	}
	this->removeTool(tool);
	delete tool;
}

void ToolManager::removeTool(otoolkit::Tool* _tool) {
	QString toolName = _tool->toolName();

	// Remove from list
	auto it = m_tools.find(toolName);
	if (it == m_tools.end()) {
		OT_LOG_W("Tool not found { \"Name\": \"" + toolName.toStdString() + "\" }");
		return;
	}
	m_tools.erase(it);

	// Clean up data
	this->fwdRemoveTool(toolName);
}

void ToolManager::clear(void) {
	// Clear all tools
	for (auto t : m_tools) {
		if (t.second) {
			delete t.second;
		}
		this->fwdRemoveTool(t.first);
	}
	
	m_tools.clear();
}

void ToolManager::stopAll(void) {
	std::map<QString, ToolRuntimeHandler*> bak = m_tools;
	for (auto it : bak) {
		//this->removeTool(it.first);
		this->stopTool(it.first);
	}
}

void ToolManager::stopTool(const QString& _toolName) {
	auto it = m_tools.find(_toolName);
	if (it == m_tools.end()) {
		OT_LOG_W("Tool not found { \"Name\": \"" + _toolName.toStdString() + "\" }");
		return;
	}

	if (!it->second->isRunning()) {
		return;
	}

	QSettings settings("OpenTwin", "OToolkit");

	it->second->setStopped();
	it->second->tool()->prepareToolShutdown(settings);
	this->fwdRemoveTool(_toolName);
}

void ToolManager::runToolTriggered(void) {
	ToolMenuManager* tmm = dynamic_cast<ToolMenuManager*>(sender());
	if (tmm == nullptr) {
		OT_LOG_E("Invalid sender");
		return;
	}

	auto it = m_tools.find(tmm->toolName());
	if (it == m_tools.end()) {
		OT_LOG_E("Tool not found { \"ToolName\": \"" + tmm->toolName().toStdString() + "\" }");
		return;
	}

	if (it->second->isRunning()) {
		// Stop the tool



		// ....



	}
	else {
		m_ignoreEvents = true;
		QSettings settings("OpenTwin", "OToolkit");
		otoolkit::ToolWidgets data;
		if (!it->second->tool()->runTool(tmm, data)) {
			OT_LOG_W("Tool \"" + it->first.toStdString() + "\" statup failed");
			return;
		}
		
		for (ot::WidgetView* view : data.views()) {
			ot::WidgetViewManager::instance().addView(ot::BasicServiceInformation(it->second->tool()->toolName().toStdString()), view);
		}

		it->second->setRunning();

		m_statusManager->addTool(it->first, data.statusWidgets());
		m_toolBarManager->addTool(it->first, data.toolBar());
		it->second->tool()->restoreToolSettings(settings);
		
		m_ignoreEvents = false;
	}
}

void ToolManager::currentToolChanged(const QString& _toolName) {
	if (m_ignoreEvents) return;
	m_statusManager->setCurrentTool(_toolName);
	m_toolBarManager->setCurrentTool(_toolName);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ToolManager::fwdRemoveTool(const QString& _toolName) {
	ot::WidgetViewManager::instance().closeViews(ot::BasicServiceInformation(_toolName.toStdString()));

	m_statusManager->removeTool(_toolName);
	m_toolBarManager->removeTool(_toolName);
}