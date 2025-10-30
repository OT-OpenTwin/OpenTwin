// @otlicense
// File: TreeItemDelegate.h
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

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qstyleditemdelegate.h>

class QTreeWidget;

namespace ot {

	class OT_WIDGETS_API_EXPORT TreeItemDelegate : public QStyledItemDelegate {
		OT_DECL_NODEFAULT(TreeItemDelegate)
		OT_DECL_NOCOPY(TreeItemDelegate)
		OT_DECL_DEFMOVE(TreeItemDelegate)
	public:
		TreeItemDelegate(QTreeWidget* _tree);
		virtual ~TreeItemDelegate();

		virtual QWidget* createEditor(QWidget* _parent, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

	private:
		QTreeWidget* m_tree;
	};

}