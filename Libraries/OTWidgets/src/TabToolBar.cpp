//! @file TabToolBar.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/MainWindow.h"
#include "OTWidgets/TabToolBar.h"
#include "OTWidgets/TabToolBarPage.h"

// TabToolBar header
#include <TabToolbar/TabToolbar.h>

ot::TabToolBar::TabToolBar(MainWindow* _window)
	: m_toolBar(nullptr)
{
	m_toolBar = new tt::TabToolbar;

	if (_window) {
		_window->addToolBar(m_toolBar);
	}
}

ot::TabToolBar::~TabToolBar() {
	for (TabToolBarPage* page : m_pages) {
		page->setParentTabToolBar(nullptr);
		delete page;
	}

	delete m_toolBar;
	m_toolBar = nullptr;
}

QToolBar* ot::TabToolBar::getToolBar(void) {
	return m_toolBar;
}

const QToolBar* ot::TabToolBar::getToolBar(void) const {
	return m_toolBar;
}

void ot::TabToolBar::forgetPage(TabToolBarPage* _page) {
	auto it = std::find(m_pages.begin(), m_pages.end(), _page);
	if (it != m_pages.end()) {
		m_pages.erase(it);
	}
}
