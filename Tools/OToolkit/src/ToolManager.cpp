//! @file ToolManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "ToolManager.h"
#include "TabManager.h"
#include "DockManager.h"
#include "MenuManager.h"
#include "StatusManager.h"
#include "ToolMenuManager.h"
#include "ToolRuntimeHandler.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OpenTwinCore/otAssert.h"

#define TOOLMANAGER_LOG(___msg) OTOOLKIT_LOG("ToolManager", ___msg)
#define TOOLMANAGER_LOGW(___msg) OTOOLKIT_LOGW("ToolManager", ___msg)
#define TOOLMANAGER_LOGE(___msg) OTOOLKIT_LOGE("ToolManager", ___msg)

ToolManager::ToolManager()
	: m_statusManager(nullptr), m_dockManager(nullptr)
{

}

ToolManager::~ToolManager() {
	this->clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

bool ToolManager::addTool(otoolkit::Tool* _tool) {
	otoolkit::Tool* t = this->findTool(_tool->toolName());
	if (t != nullptr) {
		TOOLMANAGER_LOGW("Tool already exists");
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
		TOOLMANAGER_LOGE("Tool not found { \"Name\": \"" + _toolName + "\" }");
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
		TOOLMANAGER_LOGW("Tool not found { \"Name\": \"" + toolName + "\" }");
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

void ToolManager::runToolTriggered(void) {
	ToolMenuManager* tmm = dynamic_cast<ToolMenuManager*>(sender());
	if (tmm == nullptr) {
		TOOLMANAGER_LOGE("Invalid sender");
		return;
	}

	auto it = m_tools.find(tmm->toolName());
	if (it == m_tools.end()) {
		TOOLMANAGER_LOGE("Tool not found { \"ToolName\": \"" + tmm->toolName() + "\" }");
		return;
	}

	if (it->second->isRunning()) {
		// Stop the tool



		// ....



	}
	else {
		m_tabManager->addTool(it->first, it->second->tool()->runTool(tmm));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ToolManager::fwdRemoveTool(const QString& _toolName) {
	m_dockManager->removeTool(_toolName);
	m_statusManager->removeTool(_toolName);
}