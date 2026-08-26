// @otlicense
// File: JsonTreeWidgetFilterModel.cpp
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
#include "OTWidgets/Widgets/JsonTreeWidgetFilterModel.h"

ot::JsonTreeWidgetFilterModel::JsonTreeWidgetFilterModel(QObject* _parent)
	: QSortFilterProxyModel(_parent)
{
	setRecursiveFilteringEnabled(true);
	setAutoAcceptChildRows(true);
}

void ot::JsonTreeWidgetFilterModel::setTextFilter(const QString& _text)
{
	setFilterRegularExpression(QRegularExpression(QRegularExpression::escape(_text), QRegularExpression::CaseInsensitiveOption));
}

bool ot::JsonTreeWidgetFilterModel::filterAcceptsRow(int _sourceRow, const QModelIndex& _sourceParent) const
{
	const QRegularExpression filter = filterRegularExpression();
	if (!filter.isValid())
	{
		return true;
	}

	QModelIndex keyIdx = sourceModel()->index(_sourceRow, 0, _sourceParent);
	QModelIndex valueIdx = sourceModel()->index(_sourceRow, 1, _sourceParent);
	if (!keyIdx.isValid())
	{
		return false;
	}
	if (!valueIdx.isValid())
	{
		return false;
	}

	const QString key = keyIdx.data(Qt::DisplayRole).toString();
	const QString value = valueIdx.data(Qt::DisplayRole).toString();
	bool result = key.contains(filter) || value.contains(filter);

	return result;

}
