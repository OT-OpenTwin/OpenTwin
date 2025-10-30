// @otlicense
// File: NavigationTreeView.cpp
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
#include "NavigationTreeView.h"

// uiCore header
#include "akWidgets/aTreeWidget.h"

// Qt header
#include <QtWidgets/qshortcut.h>

ot::NavigationTreeView::NavigationTreeView()
	: WidgetView(WidgetViewBase::ViewNavigation)
{
	m_tree = new ak::aTreeWidget;
	this->addWidgetInterfaceToDock(m_tree);

	QShortcut* projectNavCopy = new QShortcut(QKeySequence("Ctrl+C"), m_tree->getQWidget());
	projectNavCopy->setContext(Qt::WidgetWithChildrenShortcut);
	this->connect(projectNavCopy, &QShortcut::activated, this, &NavigationTreeView::slotCopyRequested);
	QShortcut* projectNavPaste = new QShortcut(QKeySequence("Ctrl+P"), m_tree->getQWidget());
	projectNavPaste->setContext(Qt::WidgetWithChildrenShortcut);
	this->connect(projectNavPaste, &QShortcut::activated, this, &NavigationTreeView::slotPasteRequested);
}

ot::NavigationTreeView::~NavigationTreeView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::NavigationTreeView::getViewWidget(void) {
	return m_tree->widget();
}

void ot::NavigationTreeView::slotCopyRequested(void) {
	CopyInformation info;
	info.setOriginViewInfo(this->getViewData());

	Q_EMIT copyRequested(info);
}

void ot::NavigationTreeView::slotPasteRequested(void) {
	CopyInformation info;
	info.setDestinationViewInfo(this->getViewData());

	Q_EMIT pasteRequested(info);
}
