// @otlicense
// File: JsonTreeWidget.h
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
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/JsonTreeWidgetNode.h"
#include "OTWidgets/JsonTreeWidgetModel.h"
#include "OTWidgets/JsonTreeWidgetFilterModel.h"

// Qt header
#include <QtWidgets/qtreeview.h>

namespace ot {

    class OT_WIDGETS_API_EXPORT JsonTreeWidget : public QTreeView {
        Q_OBJECT
        OT_DECL_NOCOPY(JsonTreeWidget)
        OT_DECL_NOMOVE(JsonTreeWidget)
        OT_DECL_NODEFAULT(JsonTreeWidget)
    public:
        JsonTreeWidget(QWidget* _parent);
		virtual ~JsonTreeWidget();

        void setFromJsonDocument(const JsonDocument& _doc);
		JsonDocument toJsonDocument() const;

        void setReadOnly(bool _readOnly) { m_readOnly = _readOnly; };
		bool isReadOnly() const { return m_readOnly; };

    Q_SIGNALS:
		void nodeDoubleClicked(int _column, ot::JsonTreeWidgetNode* _node);

    public Q_SLOTS:
		void filterItems(const QString& _filterText);

    protected:
		virtual bool edit(const QModelIndex& _index, QAbstractItemView::EditTrigger _trigger, QEvent* _event) override;

    private Q_SLOTS:
		void slotItemExpanded(const QModelIndex& _index);
		void slotDoubleClicked(const QModelIndex& _index);

    private:
        JsonTreeWidgetModel* m_model;
        JsonTreeWidgetFilterModel* m_filterModel;
        bool m_readOnly;
    };

}
