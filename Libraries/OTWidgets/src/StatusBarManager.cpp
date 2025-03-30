//! @file StatusBarManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/StatusBarManager.h"

// Qt header
#include <QtWidgets/qstatusbar.h>

ot::StatusBarManager::StatusBarManager() :
	m_statusBar(nullptr)
{
	m_statusBar = new QStatusBar;
}

ot::StatusBarManager::~StatusBarManager() {
	delete m_statusBar;
	m_statusBar = nullptr;
}

QStatusBar* ot::StatusBarManager::getStatusBar(void) {
	return m_statusBar;
}

const QStatusBar* ot::StatusBarManager::getStatusBar(void) const {
	return m_statusBar;
}
