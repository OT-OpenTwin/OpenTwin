// @otlicense
// File: WidgetViewDock.cpp
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
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/WidgetViewTab.h"
#include "OTWidgets/WidgetViewDock.h"

ot::WidgetViewDock::WidgetViewDock(WidgetView* _view, QWidget* _parent) :
	ads::CDockWidget(QString(), _parent), m_view(_view), m_tab(nullptr)
{
	m_tab = dynamic_cast<WidgetViewTab*>(this->tabWidget());
	if (m_tab) {
		this->connect(m_tab, &WidgetViewTab::viewCloseRequested, this, &WidgetViewDock::slotCloseRequested);
		this->connect(m_tab, &WidgetViewTab::viewPinnedChanged, this, &WidgetViewDock::slotPinnedChanged);
	}
	else {
		OT_LOG_EA("Unexpected tab");
	}

	this->setFeature(ads::CDockWidget::CustomCloseHandling, true);
	this->setFeature(ads::CDockWidget::DockWidgetClosable, false);

	this->connect(this, &ads::CDockWidget::closeRequested, this, &WidgetViewDock::slotCloseRequested);
}

ot::WidgetViewDock::~WidgetViewDock() {
	
}

void ot::WidgetViewDock::openView(void) {
	this->toggleView(true);
}

void ot::WidgetViewDock::closeView(void) {
	this->toggleView(false);
}

void ot::WidgetViewDock::setCloseButtonVisible(bool _vis) {
	OTAssertNullptr(m_tab);
	m_tab->setCloseButtonVisible(_vis);
}

void ot::WidgetViewDock::setPinButtonVisible(bool _vis) {
	OTAssertNullptr(m_tab);
	m_tab->setPinButtonVisible(_vis);
}

void ot::WidgetViewDock::setIsPinned(bool _isPinned) {
	OTAssertNullptr(m_tab);
	m_tab->setIsPinned(_isPinned);
}

bool ot::WidgetViewDock::getIsPinned(void) const {
	OTAssertNullptr(m_tab);
	return m_tab->getIsPinned();
}

void ot::WidgetViewDock::resizeEvent(QResizeEvent* _event) {
	ads::CDockWidget::resizeEvent(_event);
	Q_EMIT dockResized(_event->size());
}

void ot::WidgetViewDock::slotCloseRequested(void) {
	Q_EMIT dockCloseRequested();
}

void ot::WidgetViewDock::slotPinnedChanged(bool _isPinned) {
	Q_EMIT dockPinnedChanged(_isPinned);
}
