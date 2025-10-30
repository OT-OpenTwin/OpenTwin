// @otlicense
// File: PropertyGridItemDelegate.cpp
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
#include "OTWidgets/TreeWidget.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/PropertyGridItemDelegate.h"

// Qt header
#include <QtGui/qpainter.h>

ot::PropertyGridItemDelegate::PropertyGridItemDelegate(TreeWidget* _tree) 
	: QStyledItemDelegate(_tree), m_tree(_tree)
{
	OTAssertNullptr(m_tree);
	m_tree->setItemDelegate(this);
}

ot::PropertyGridItemDelegate::~PropertyGridItemDelegate() {
	m_tree->setItemDelegate(nullptr);
	m_tree = nullptr;
}

void ot::PropertyGridItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
	PropertyGridItem* itm = dynamic_cast<PropertyGridItem*>(m_tree->itemFromIndex(_index));
	PropertyGridGroup* grp = dynamic_cast<PropertyGridGroup*>(m_tree->itemFromIndex(_index));

	if (itm) {
		_painter->fillRect(_option.rect, GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::WidgetBackground).toBrush());
	}
	else if (grp) {
		_painter->fillRect(_option.rect, GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::TitleBackground).toBrush());
	}
	else {
		OT_LOG_E("Unknown property grid entry");
	}

	QStyledItemDelegate::paint(_painter, _option, _index);
}