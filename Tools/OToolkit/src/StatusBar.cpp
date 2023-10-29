//! @file StatusBar.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "StatusBar.h"

#include <QtCore/qtimer.h>
#include <QtWidgets/qlabel.h>

StatusBar::StatusBar() : m_statusText("Ready"), m_statusIsError(false) {
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

