//! @file StatusBar.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "ToolBarManager.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTCore/Logger.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qmainwindow.h>

ToolBarManager::ToolBarManager(QMainWindow* _window)
	: m_window(_window)
{

}

ToolBarManager::~ToolBarManager() {

}

void ToolBarManager::setCurrentTool(const QString& _toolName) {
	// Clean up current tool
	if (!m_currentTool.isEmpty()) {
		auto cit = m_toolBars.find(m_currentTool);
		if (cit == m_toolBars.end()) {
			OT_LOG_E("Tool not found for cleanup { \"ToolName\": \"" + m_currentTool.toStdString() + "\" }");
			// Reset the current tool and exit
			m_currentTool = "";
			return;
		}

		if (cit->second) {
			cit->second->setHidden(true);
			m_window->removeToolBar(cit->second);
		}
	}

	m_currentTool = _toolName;

	// Apply new tool
	bool setVis = false;
	if (!m_currentTool.isEmpty()) {
		auto it = m_toolBars.find(m_currentTool);
		if (it == m_toolBars.end()) {
			OT_LOG_E("Tool not found { \"ToolName\": \"" + m_currentTool.toStdString() + "\" }");
			return;
		}

		if (it->second) {
			m_window->addToolBar(it->second);
			it->second->setHidden(false);
		}
	}
}

void ToolBarManager::addTool(const QString& _toolName, QToolBar* _toolBar) {
	// Ensure that the tool did not register already
	auto it = m_toolBars.find(_toolName);
	if (it != m_toolBars.end()) {
		OT_LOG_E("Tool already exists { \"ToolName\": \"" + _toolName.toStdString() + "\" }");
		return;
	}

	m_toolBars.insert_or_assign(_toolName, _toolBar);

	if (m_currentTool.isEmpty()) {
		this->setCurrentTool(_toolName);
	}
}

void ToolBarManager::removeTool(const QString& _toolName) {
	// Check if the tool to remove is the active one
	if (_toolName == m_currentTool) {
		// Set empty current tool (will clear the current tool only)
		this->setCurrentTool("");
	}

	m_toolBars.erase(_toolName); 
}
