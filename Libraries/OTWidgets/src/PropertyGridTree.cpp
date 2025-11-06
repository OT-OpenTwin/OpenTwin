// @otlicense
// File: PropertyGridTree.cpp
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
#include "OTWidgets/PropertyGridTree.h"
#include "OTWidgets/PropertyGridItemDelegate.h"

// Qt header
#include <QtGui/qevent.h>

ot::PropertyGridTree::PropertyGridTree(QWidget* _parent) : TreeWidget(_parent), m_wasShown(false), m_delegate(nullptr) {
	this->setColumnCount(2);
	this->setHeaderLabels({ "Name", "Value" });
	this->setIndentation(0);
	this->setObjectName("ot_property_grid");

	m_delegate = new PropertyGridItemDelegate(this);
}

void ot::PropertyGridTree::mousePressEvent(QMouseEvent* _event) {
	QModelIndex index = indexAt(_event->pos());
	bool last_state = isExpanded(index);

	TreeWidget::mousePressEvent(_event);

	if (index.isValid() && last_state == isExpanded(index) && this->itemFromIndex(index) && this->itemFromIndex(index)->childCount() > 0) {
		this->setExpanded(index, !last_state);
	}

}

void ot::PropertyGridTree::showEvent(QShowEvent* _event) {
	if (!m_wasShown) {
		this->setColumnWidth(0, this->width() / 2);
		m_wasShown = true;
	}
	TreeWidget::showEvent(_event);
}

void ot::PropertyGridTree::resizeEvent(QResizeEvent* _event) {
	TreeWidget::resizeEvent(_event);
}

void ot::PropertyGridTree::drawRow(QPainter* _painter, const QStyleOptionViewItem& _options, const QModelIndex& _index) const {
	TreeWidget::drawRow(_painter, _options, _index);
}
