// @otlicense
// File: TreeItemViewLink.cpp
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
#include "OTSystem/OTAssert.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/WidgetViewDock.h"
#include "OTWidgets/TreeItemViewLink.h"
#include "OTWidgets/WidgetViewManager.h"
#include "OTWidgets/WidgetViewManager.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

ot::TreeItemViewLink::TreeItemViewLink(QTreeWidgetItem* _treeItem, WidgetView* _view, const LinkFlags& _flags) :
	m_view(_view), m_treeItem(_treeItem), m_flags(_flags)
{
	OTAssertNullptr(m_view);
	OTAssertNullptr(m_treeItem);

	this->connect(m_treeItem->treeWidget(), &QTreeWidget::itemSelectionChanged, this, &TreeItemViewLink::slotTreeSelectionChanged);
	this->connect(&WidgetViewManager::instance(), &WidgetViewManager::viewFocusChanged, this, &TreeItemViewLink::slotViewFocusChanged);
}

ot::TreeItemViewLink::~TreeItemViewLink() {

}

void ot::TreeItemViewLink::slotTreeSelectionChanged() {
	if (m_treeItem->isSelected() && (m_flags & LinkFlag::OpenOnSelect) && !m_view->getViewDockWidget()->isVisible()) {
		m_view->openView();
	}
	else if (!m_treeItem->isSelected() && (m_flags & LinkFlag::CloseOnDeselect) && m_view->getViewDockWidget()->isVisible()) {
		m_view->closeView();
	}
}

void ot::TreeItemViewLink::slotViewFocusChanged(WidgetView* _focusedView, WidgetView* _previousView) {
	if (_focusedView) {
		if ((m_flags & CentralViewsOnly) && !(_focusedView->getViewData().getViewFlags() & WidgetViewBase::ViewIsCentral)) {
			return;
		}
	}
	
	if ((m_flags & LinkFlag::SelectOnFocus) && _previousView == m_view && m_treeItem->isSelected()) {
		m_treeItem->setSelected(false);
	}
	else if ((m_flags & LinkFlag::DeselectOnFocusLost) && _focusedView == m_view && !m_treeItem->isSelected()) {
		m_treeItem->setSelected(true);
	}
}

