#include "StatusBar.h"

#include "AbstractTool.h"

#include <QtCore/qtimer.h>
#include <QtWidgets/qlabel.h>

StatusBar::StatusBar() : m_statusText("Ready"), m_statusIsError(false), m_currentTool(nullptr) {
	// Create labels
	m_infoLabel = new QLabel(m_statusText, this);
	m_stretchLabel = new QLabel;

	addWidget(m_infoLabel);
	addWidget(m_stretchLabel, 1);
	
	// Initialize color
	setStyleSheet(
		"QStatusBar{ color: white; background-color: #00A2E8; } "
		"QLabel { color: white; }"
	);
	
	// Create timer
	m_timerErrorStatusReset = new QTimer;
	m_timerErrorStatusReset->setInterval(5000);
	m_timerErrorStatusReset->setSingleShot(true);
	connect(m_timerErrorStatusReset, &QTimer::timeout, this, &StatusBar::slotResetErrorStatus);
}

void StatusBar::setCurrentTool(OToolkitAPI::AbstractTool * _tool) {
	if (_tool == m_currentTool) {
		return;
	}

	removeCurrentToolWidgets();

	// Ensure that the tool is stored
	auto it = m_toolWidgets.find(_tool);
	if (it == m_toolWidgets.end()) {
		m_toolWidgets.insert_or_assign(_tool, _tool->statusBarWidgets());
	}

	for (QWidget * w : _tool->statusBarWidgets()) {
		addWidget(w);
	}
}

void StatusBar::toolDestroyed(OToolkitAPI::AbstractTool * _tool) {
	if (_tool == nullptr) { return; }
	if (_tool == m_currentTool) {
		removeCurrentToolWidgets();
	}
	m_toolWidgets.erase(_tool);
}

void StatusBar::setInfo(const QString& _text) {
	m_statusText = _text;
	if (m_statusIsError) return;
	else m_infoLabel->setText(m_statusText);
}

void StatusBar::setErrorInfo(const QString& _text) {
	m_infoLabel->setText(_text);
	m_timerErrorStatusReset->stop();
	m_timerErrorStatusReset->start();

	setStyleSheet(
		"QStatusBar{ color: white; background-color: #F05F00; } "
		"QLabel{ color: white; }"
	);
}

void StatusBar::slotResetErrorStatus(void) {
	QString labelStyleSheet("QLabel { color: white; }");
	setStyleSheet("QStatusBar{ color: white; background-color: #00A2E8; }");
	m_infoLabel->setStyleSheet(labelStyleSheet);
	
	m_infoLabel->setText(m_statusText);
}

void StatusBar::removeCurrentToolWidgets(void) {
	if (m_currentTool) {
		// Remove old widgets
		auto it = m_toolWidgets.find(m_currentTool);
		if (it == m_toolWidgets.end()) {
			assert(0); // The current tool is not stored in the list
			return;
		}
		for (QWidget * w : it->second) {
			w->setParent(nullptr);
			removeWidget(w);
		}
	}
	m_currentTool = nullptr;
}
