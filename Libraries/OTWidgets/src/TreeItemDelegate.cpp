// @otlicense
// File: TreeItemDelegate.cpp
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
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/TreeItemDelegate.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

ot::TreeItemDelegate::TreeItemDelegate(QTreeWidget* _tree) : m_tree(_tree) {
	OTAssertNullptr(m_tree);
	m_tree->setItemDelegate(this);
}

ot::TreeItemDelegate::~TreeItemDelegate() {
	m_tree->setItemDelegate(nullptr);
	m_tree = nullptr;
}

QWidget* ot::TreeItemDelegate::createEditor(QWidget* _parent, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
	LineEdit* newEdit = new LineEdit(_parent);
	newEdit->setObjectName("OT_TreeItemEdit");
	return newEdit;
}
