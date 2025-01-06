//! @file TabToolBar.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/MainWindow.h"
#include "OTWidgets/TabToolBar.h"
#include "OTWidgets/TabToolBarPage.h"

// TabToolBar header
#include <TabToolbar/Page.h>
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

ot::TabToolBarPage* ot::TabToolBar::addPage(const std::string& _pageName) {
	if (this->hasPage(_pageName)) {
		OT_LOG_E("Page \"" + _pageName + "\" already exists");
		return nullptr;
	}
	
	tt::Page* page = m_toolBar->AddPage(QString::fromStdString(_pageName));

	TabToolBarPage* newPage = new TabToolBarPage(this, page, _pageName);
	m_pages.push_back(newPage);

	return newPage;
}

void ot::TabToolBar::forgetPage(TabToolBarPage* _page) {
	auto it = std::find(m_pages.begin(), m_pages.end(), _page);
	if (it != m_pages.end()) {
		m_pages.erase(it);
	}
}

ot::TabToolBarPage* ot::TabToolBar::findPage(const std::string& _pageName) {
	for (TabToolBarPage* page : m_pages) {
		if (page->getName() == _pageName) {
			return page;
		}
	}
	return nullptr;
}

const ot::TabToolBarPage* ot::TabToolBar::findPage(const std::string& _pageName) const {
	for (const TabToolBarPage* page : m_pages) {
		if (page->getName() == _pageName) {
			return page;
		}
	}
	return nullptr;
}

ot::TabToolBarPage* ot::TabToolBar::findPageFromTitle(const QString& _pageTitle) {
	for (TabToolBarPage* page : m_pages) {
		if (page->getTitle() == _pageTitle) {
			return page;
		}
	}
	return nullptr;
}
