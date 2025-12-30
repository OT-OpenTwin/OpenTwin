// @otlicense
// File: JsonTreeWidgetFilterModel.h
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
#include <QtCore/qsortfilterproxymodel.h>

namespace ot {
	
	class OT_WIDGETS_API_EXPORT JsonTreeWidgetFilterModel : public QSortFilterProxyModel {
		OT_DECL_NOCOPY(JsonTreeWidgetFilterModel)
		OT_DECL_NOMOVE(JsonTreeWidgetFilterModel)
		OT_DECL_NODEFAULT(JsonTreeWidgetFilterModel)
    public:
        JsonTreeWidgetFilterModel(QObject* _parent);

        void setFilterText(const QString& _text);

    protected:
        bool filterAcceptsRow(int _sourceRow, const QModelIndex& _sourceParent) const override;

    private:
        QString m_filterText;

        bool rowMatches(const QModelIndex& _index) const;

        bool hasMatchingParent(QModelIndex _parent) const;

        bool hasMatchingChildren(const QModelIndex& _parent) const;
	};

}