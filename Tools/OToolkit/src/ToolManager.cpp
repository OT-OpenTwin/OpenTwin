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
#include "ToolViewManager.h"
#include "ToolRuntimeHandler.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/BasicServiceInformation.h"

ToolManager::ToolManager(QMainWindow* _mainWindow)
	: m_ignoreEvents(false)
{
	m_menuManager = new MenuManager;
	m_statusManager = new StatusManager;
	m_toolBarManager = new ToolBarManager(_mainWindow);
	m_toolViewManager = new ToolViewManager;

	this->connect(m_toolViewManager, &ToolViewManager::viewFocused, this, &ToolManager::slotViewFocused);
	this->connect(m_toolViewManager, &ToolViewManager::viewFocusLost, this, &ToolManager::slotViewFocusLost);
	this->connect(m_toolViewManager, &ToolViewManager::viewCloseRequested, this, &ToolManager::slotViewCloseRequested);
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
	m_tools.insert_or_assign(_tool->toolName(), new ToolRuntimeHandler(_tool, this));

	// Create menu and connect the run signal
	ToolMenuManager* tmm = m_menuManager->addToolMenu(_tool->toolName());
	this->connect(tmm, &ToolMenuManager::runRequested, this, &ToolManager::slotRunToolTriggered);

	return true;
}

otoolkit::Tool* ToolManager::findTool(const QString& _toolName) {
	auto it = m_tools.find(_toolName);
	if (it == m_tools.end()) {
		return nullptr;
	}
	return it->second->getTool();
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

// ###########################################################################################################################################################################################################################################################################################################################

// Runtime handling

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
	it->second->getTool()->prepareToolShutdown(settings);
	this->fwdRemoveTool(_toolName);
}

void ToolManager::toolDataHasChanged(ToolRuntimeHandler* _handler) {
	OTAssertNullptr(_handler->getTool());
	m_toolViewManager->updateViews(_handler->getTool()->toolName());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ToolManager::slotRunToolTriggered(void) {
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
		OT_LOG_W("Stop not implemented. Restart the application to restart the tool.");
		// Stop the tool



		// ....



	}
	else {
		m_ignoreEvents = true;
		QSettings settings("OpenTwin", "OToolkit");
		if (!it->second->getTool()->runTool(tmm, it->second->getToolWidgets())) {
			OT_LOG_W("Tool \"" + it->first.toStdString() + "\" statup failed");
			return;
		}

		it->second->setRunning();

		m_statusManager->addTool(it->first, it->second->getToolWidgets().getStatusWidgets());
		m_toolBarManager->addTool(it->first, it->second->getToolWidgets().getToolBar());
		m_toolViewManager->addTool(it->second);

		it->second->getTool()->restoreToolSettings(settings);

		m_ignoreEvents = false;

		it->second->notifyDataChanged();
	}
}

void ToolManager::slotViewFocused(const QString& _viewName, const QString& _toolName) {
	if (m_ignoreEvents) return;

	m_statusManager->setCurrentTool(_toolName);
	m_toolBarManager->setCurrentTool(_toolName);
}

void ToolManager::slotViewFocusLost(const QString& _viewName, const QString& _toolName) {

}

void ToolManager::slotViewCloseRequested(const QString& _viewName, const QString& _toolName) {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ToolManager::fwdRemoveTool(const QString& _toolName) {
	m_toolViewManager->removeTool(_toolName);
	m_statusManager->removeTool(_toolName);
	m_toolBarManager->removeTool(_toolName);
}