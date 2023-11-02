//! @file StatusBar.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "StatusManager.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtWidgets/qlabel.h>

#define STATUS_LOG(___msg) OTOOLKIT_LOG("StatusManager", ___msg)
#define STATUS_LOGW(___msg) OTOOLKIT_LOGW("StatusManager", ___msg)
#define STATUS_LOGE(___msg) OTOOLKIT_LOGE("StatusManager", ___msg)

StatusManager::StatusManager() : m_statusText("Ready"), m_statusIsError(false) {
	// Create labels
	m_infoLabel = new QLabel(m_statusText, this);
	m_stretchLabel = new QLabel;

	this->addWidget(m_infoLabel);
	this->addWidget(m_stretchLabel, 1);
	
	// Initialize color
	setStyleSheet(
		"QStatusBar{ color: white; background-color: #00A2E8; } "
		"QLabel { color: white; }"
	);
	
	// Create timer
	m_timerErrorStatusReset = new QTimer;
	m_timerErrorStatusReset->setInterval(5000);
	m_timerErrorStatusReset->setSingleShot(true);
	connect(m_timerErrorStatusReset, &QTimer::timeout, this, &StatusManager::slotResetErrorStatus);
}

StatusManager::~StatusManager() {

}

void StatusManager::setInfo(const QString& _text) {
	m_statusText = _text;
	if (m_statusIsError) return;
	else m_infoLabel->setText(m_statusText);
}

void StatusManager::setErrorInfo(const QString& _text) {
	m_infoLabel->setText(_text);
	m_timerErrorStatusReset->stop();
	m_timerErrorStatusReset->start();

	setStyleSheet(
		"QStatusBar{ color: white; background-color: #F05F00; } "
		"QLabel{ color: white; }"
	);
}

void StatusManager::setCurrentTool(const QString& _toolName) {
	// Clean up current tool
	if (!m_currentTool.isEmpty()) {
		auto cit = m_toolWidgets.find(m_currentTool);
		if (cit == m_toolWidgets.end()) {
			STATUS_LOGE("Tool not found for cleanup { \"ToolName\": \"" + m_currentTool + "\" }");
			// Reset the current tool and exit
			m_currentTool = "";
			return;
		}

		for (auto w : cit->second) {
			// Hide the widget
			w->hide();

			// Remove the widget from the statusbar
			this->removeWidget(w);
		}
	}

	m_currentTool = _toolName;

	// Apply new tool
	if (!m_currentTool.isEmpty()) {
		auto it = m_toolWidgets.find(m_currentTool);
		if (it == m_toolWidgets.end()) {
			STATUS_LOGE("Tool not found { \"ToolName\": \"" + m_currentTool + "\" }");
			return;
		}

		for (auto w : it->second) {
			// Add and show widgets (widgets may be hidden due to previous remove)
			this->addWidget(w);
			w->show();
		}
	}
}

void StatusManager::addTool(const QString& _toolName, const QList<QWidget*>& _widgets) {
	// Ensure that the tool did not register already
	auto it = m_toolWidgets.find(_toolName);
	if (it != m_toolWidgets.end()) {
		STATUS_LOGE("Tool already exists { \"ToolName\": \"" + _toolName + "\" }");
		return;
	}

	m_toolWidgets.insert_or_assign(_toolName, _widgets);
}

void StatusManager::removeTool(const QString& _toolName) {
	// Check if the tool to remove is the active one
	if (_toolName == m_currentTool) {
		// Set empty current tool (will clear the current tool only)
		this->setCurrentTool("");
	}

	m_toolWidgets.erase(_toolName);
}

void StatusManager::slotResetErrorStatus(void) {
	QString labelStyleSheet("QLabel { color: white; }");
	setStyleSheet("QStatusBar{ color: white; background-color: #00A2E8; }");
	m_infoLabel->setStyleSheet(labelStyleSheet);
	
	m_infoLabel->setText(m_statusText);
}

