//! @file ToolManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OToolkit header
#include "ToolManager.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OpenTwinCore/otAssert.h"

#define TOOLMANAGER_LOG(___msg) OTOOLKIT_LOG("ToolManager", ___msg)
#define TOOLMANAGER_LOGW(___msg) OTOOLKIT_LOGW("ToolManager", ___msg)
#define TOOLMANAGER_LOGE(___msg) OTOOLKIT_LOGE("ToolManager", ___msg)

ToolManager& ToolManager::instance(void) {
	static ToolManager g_instance{ };
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ToolManager::addTool(otoolkit::Tool* _tool) {
	for (otoolkit::Tool* t : m_tools) {
		if (t->toolName() == _tool->toolName()) {
			TOOLMANAGER_LOGW("Tool already exists");
			return;
		}
	}
	m_tools.push_back(_tool);
}

otoolkit::Tool* ToolManager::findTool(const QString& _toolName) {
	for (otoolkit::Tool* t : m_tools) {
		if (t->toolName() == _toolName) {
			return t;
		}
	}
	return nullptr;
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
	// Remove from list
	auto it = std::find(m_tools.begin(), m_tools.end(), _tool);
	if (it == m_tools.end()) {
		TOOLMANAGER_LOGW("Tool not found { \"Name\": \"" + _tool->toolName() + "\" }");
		return;
	}
	m_tools.erase(it);

	// Clean up data
	
}

void ToolManager::clear(void) {
	// Clear all tools
	for (otoolkit::Tool* t : m_tools) {
		if (t) {
			delete t;
		}
	}
	m_tools.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

ToolManager::ToolManager() {

}

ToolManager::~ToolManager() {
	this->clear();
}