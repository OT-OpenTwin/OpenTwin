// @otlicense
// File: JsonTreeWidgetModel.h
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
#include "OTWidgets/JsonTreeWidgetNode.h"

// Qt header
#include <QtCore/qvariant.h>
#include <QtCore/qabstractitemmodel>

namespace ot {

    class OT_WIDGETS_API_EXPORT JsonTreeWidgetModel : public QAbstractItemModel {
        Q_OBJECT
		OT_DECL_NOCOPY(JsonTreeWidgetModel)
		OT_DECL_NOMOVE(JsonTreeWidgetModel)
		OT_DECL_NODEFAULT(JsonTreeWidgetModel)
    public:
        enum ColumnIndex : int32_t {
            ColumnKey,
            ColumnValue,
            ColumnType,

            ColumnCount
        };

        JsonTreeWidgetModel(QObject* _parent);
        virtual ~JsonTreeWidgetModel();

        void setFromJsonDocument(const JsonDocument& _doc);
        
        // Required overrides
        QModelIndex index(int _row, int _column, const QModelIndex& _parent = QModelIndex()) const override;

        QModelIndex parent(const QModelIndex& _index) const override;

        int rowCount(const QModelIndex& _parent = QModelIndex()) const override;

        int columnCount(const QModelIndex& _parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& _index, int _role = Qt::DisplayRole) const override;

        QVariant headerData(int _section, Qt::Orientation _orientation, int _role) const override;

        Qt::ItemFlags flags(const QModelIndex& _index) const override;

        bool setData(const QModelIndex& _index, const QVariant& _value, int _role = Qt::EditRole) override;

    private:
        JsonTreeWidgetNode* m_rootNode;

    };

}
