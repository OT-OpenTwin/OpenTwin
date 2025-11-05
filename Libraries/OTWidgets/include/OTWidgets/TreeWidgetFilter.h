// @otlicense
// File: TreeWidgetFilter.h
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
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

class QWidget;
class QVBoxLayout;
class QTreeWidgetItem;

namespace ot {

	class TreeWidget;
	class LineEdit;

	class OT_WIDGETS_API_EXPORT TreeWidgetFilter : public QObject, public ot::WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(TreeWidgetFilter)
		OT_DECL_NOMOVE(TreeWidgetFilter)
		OT_DECL_NODEFAULT(TreeWidgetFilter)
	public:
		explicit TreeWidgetFilter(QWidget* _parent, ot::TreeWidget * _tree = (ot::TreeWidget *)nullptr, int _filterColumn = 0);
		virtual ~TreeWidgetFilter();

		virtual void applyFilter(const QString& _filterText);
		virtual void clearFilter(void);

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return m_layoutW; };
		virtual const QWidget* getQWidget(void) const override { return m_layoutW; };

		ot::LineEdit* getFilterLineEdit(void) { return m_lineEdit; };
		ot::TreeWidget* getTreeWidget(void) { return m_tree; };

	Q_SIGNALS:
		void filterChanged(const QString& _filterText);
		void filterCleared(void);

	public Q_SLOTS:
		void slotReturnPressed(void);
		void slotTextChanged(void);

	private:
		void dispatchFilter(const QString& _text);
		void setAllVisible(QTreeWidgetItem* _item);

		//! @return True if this item or any of its childs match the filter
		bool filterChilds(QTreeWidgetItem* _item, const QString& _filter);

		void expandAllParents(QTreeWidgetItem* _item);

		int             m_filterColumn;
		QString         m_lastFilter;
		QWidget*        m_layoutW;
		QVBoxLayout*    m_layout;

		ot::LineEdit*   m_lineEdit;
		ot::TreeWidget* m_tree;
	};
}
