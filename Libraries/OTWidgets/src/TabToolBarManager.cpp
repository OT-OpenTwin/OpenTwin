//! @file ToolBarManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TabToolBarManager.h"

// TTB header
#include <TabToolbar/TabToolbar.h>

ot::TabToolBarManager* ot::TabToolBarManager::createDefault(void) {
	TabToolBarManager* newManager = new TabToolBarManager;


	return newManager;
}

ot::TabToolBarManager::TabToolBarManager()
	: m_toolBar(nullptr)
{
	m_toolBar = new tt::TabToolbar;
}

ot::TabToolBarManager::~TabToolBarManager() {
	delete m_toolBar;
	m_toolBar = nullptr;
}

QToolBar* ot::TabToolBarManager::getToolBar(void) {
	return m_toolBar;
}

const QToolBar* ot::TabToolBarManager::getToolBar(void) const {
	return m_toolBar;
}
