// @otlicense
// File: TabToolBar.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/MainWindow.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/TabToolBar.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/TabToolBarPage.h"
#include "OTWidgets/TabToolBarButtonProxyStyle.h"

// TabToolBar header
#include <TabToolbar/Page.h>
#include <TabToolbar/TabToolbar.h>

// Qt header
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qapplication.h>

ot::ToolButton* ot::TabToolBar::createButton(const QString& _iconPath, const QString& _text, QMenu* _menu) {
	return TabToolBar::createButton(IconManager::getIcon(_iconPath), _text, _menu);
}

ot::ToolButton* ot::TabToolBar::createButton(const QIcon& _icon, const QString& _text, QMenu* _menu) {
	ToolButton* btn = new ot::ToolButton(_icon, _text);

	const int iconSize = QApplication::style()->pixelMetric(QStyle::PM_LargeIconSize);

	btn->setAutoRaise(true);
	btn->setIconSize(QSize(iconSize, iconSize));
	btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btn->setStyle(new ot::TabToolBarButtonProxyStyle());

	return btn;
}

ot::TabToolBar::TabToolBar(MainWindow* _window) :
	m_toolBar(nullptr)
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

void ot::TabToolBar::setCurrentPage(TabToolBarPage* _page) {
	QTabWidget* w = m_toolBar->tabWidget();
	for (int i = 0; i < w->count(); i++) {
		if (w->widget(i) == _page->getPage()) {
			w->setCurrentIndex(i);
			return;
		}
	}

	OTAssert(0, "Page not found");
}

ot::TabToolBarPage* ot::TabToolBar::addPage(const std::string& _pageName, bool _returnExisting) {
	TabToolBarPage* newPage = this->findPage(_pageName);
	if (newPage) {
		if (_returnExisting) {
			return newPage;
		}
		else {
			OT_LOG_E("Page \"" + _pageName + "\" already exists");
			return nullptr;
		}
	}
	
	tt::Page* page = m_toolBar->AddPage(QString::fromStdString(_pageName));

	newPage = new TabToolBarPage(this, page, _pageName);
	m_pages.push_back(newPage);

	return newPage;
}

ot::TabToolBarGroup* ot::TabToolBar::addGroup(const std::string& _pageName, const std::string& _groupName, bool _returnExisting) {
	TabToolBarPage* page = this->findPage(_pageName);
	if (!page) {
		OT_LOG_E("Page \"" + _pageName + "\" not found");
		return nullptr;
	}
	else {
		return page->addGroup(_groupName, _returnExisting);
	}
}

ot::TabToolBarSubGroup* ot::TabToolBar::addSubGroup(const std::string& _pageName, const std::string& _groupName, const std::string& _subGroupName, bool _returnExisting) {
	TabToolBarPage* page = this->findPage(_pageName);
	if (!page) {
		OT_LOG_E("Page \"" + _pageName + "\" not found");
		return nullptr;
	}
	else {
		return page->addSubGroup(_groupName, _subGroupName, _returnExisting);
	}
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

ot::TabToolBarGroup* ot::TabToolBar::findGroup(const std::string& _pageName, const std::string& _groupName) {
	TabToolBarPage* page = this->findPage(_pageName);
	if (!page) {
		OT_LOG_E("Page \"" + _pageName + "\" not found");
		return nullptr;
	}
	else {
		return page->findGroup(_groupName);
	}
}

const ot::TabToolBarGroup* ot::TabToolBar::findGroup(const std::string& _pageName, const std::string& _groupName) const {
	const TabToolBarPage* page = this->findPage(_pageName);
	if (!page) {
		OT_LOG_E("Page \"" + _pageName + "\" not found");
		return nullptr;
	}
	else {
		return page->findGroup(_groupName);
	}
}

ot::TabToolBarSubGroup* ot::TabToolBar::findSubGroup(const std::string& _pageName, const std::string& _groupName, const std::string& _subGroupName) {
	TabToolBarPage* page = this->findPage(_pageName);
	if (!page) {
		OT_LOG_E("Page \"" + _pageName + "\" not found");
		return nullptr;
	}
	else {
		return page->findSubGroup(_groupName, _subGroupName);
	}
}

const ot::TabToolBarSubGroup* ot::TabToolBar::findSubGroup(const std::string& _pageName, const std::string& _groupName, const std::string& _subGroupName) const {
	const TabToolBarPage* page = this->findPage(_pageName);
	if (!page) {
		OT_LOG_E("Page \"" + _pageName + "\" not found");
		return nullptr;
	}
	else {
		return page->findSubGroup(_groupName, _subGroupName);
	}
}
