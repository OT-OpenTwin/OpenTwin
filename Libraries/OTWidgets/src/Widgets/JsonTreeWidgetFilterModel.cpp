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
	setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void ot::JsonTreeWidgetFilterModel::setFilterText(const QString& _text)
{
	beginFilterChange();
	m_filterText = _text.trimmed();
	endFilterChange();
}

uint64_t ot::JsonTreeWidgetFilterModel::countHiddenRows() const
{
	uint64_t hiddenRows = 0;
	
	auto model = sourceModel();
	auto rootIx = model->index(0, 0, QModelIndex());
	
	if (rootIx.isValid())
	{
		countHiddenRowsRecursive(model, rootIx, hiddenRows);
	}

	return hiddenRows;
}

bool ot::JsonTreeWidgetFilterModel::filterAcceptsRow(int _sourceRow, const QModelIndex& _sourceParent) const
{
	if (m_filterText.isEmpty())
	{
		return true;
	}

	QModelIndex keyIdx = sourceModel()->index(_sourceRow, 0, _sourceParent);
	QModelIndex valueIdx = sourceModel()->index(_sourceRow, 1, _sourceParent);
	if (!keyIdx.isValid())
	{
		return false;
	}

	// 1) This row matches directly
	const QString key = keyIdx.data(Qt::DisplayRole).toString();
	const QString value = valueIdx.data(Qt::DisplayRole).toString();
	const bool selfMatches =
		key.contains(m_filterText, Qt::CaseInsensitive) ||
		value.contains(m_filterText, Qt::CaseInsensitive);

	if (selfMatches)
	{
		return true;
	}

	// 2) Parent matches
	if (hasMatchingParent(_sourceParent))
	{
		return true;
	}

	// 3) Child matches
	if (hasMatchingChildren(keyIdx))
	{
		return true;
	}

	return false;
}

void ot::JsonTreeWidgetFilterModel::countHiddenRowsRecursive(QAbstractItemModel* _model, const QModelIndex& _parent, uint64_t& _hiddenRows) const
{
	for (int r = 0; r < _model->rowCount(_parent); r++)
	{
		QModelIndex keyIdx = _model->index(r, 0, _parent);
		if (!keyIdx.isValid())
		{
			continue;
		}
		if (!filterAcceptsRow(r, _parent))
		{
			_hiddenRows++;
		}

		countHiddenRowsRecursive(_model, keyIdx, _hiddenRows);
	}
}

bool ot::JsonTreeWidgetFilterModel::rowMatches(const QModelIndex& _index) const
{
	if (!_index.isValid())
	{
		return false;
	}
	QString key = _index.sibling(_index.row(), 0).data(Qt::DisplayRole).toString();
	QString value = _index.sibling(_index.row(), 1).data(Qt::DisplayRole).toString();
	return key.contains(m_filterText, Qt::CaseInsensitive) ||
		value.contains(m_filterText, Qt::CaseInsensitive);
}

bool ot::JsonTreeWidgetFilterModel::hasMatchingParent(QModelIndex _parent) const
{
	while (_parent.isValid())
	{
		if (rowMatches(_parent))
		{
			return true;
		}
		_parent = _parent.parent();
	}
	return false;
}

bool ot::JsonTreeWidgetFilterModel::hasMatchingChildren(const QModelIndex& _parent) const
{
	const int rowCount = sourceModel()->rowCount(_parent);
	for (int r = 0; r < rowCount; ++r)
	{
		QModelIndex childKeyIdx = sourceModel()->index(r, 0, _parent);
		if (!childKeyIdx.isValid())
			continue;

		// Direct match
		if (rowMatches(childKeyIdx))
		{
			return true;
		}

		// Recursive match
		if (hasMatchingChildren(childKeyIdx))
		{
			return true;
		}
	}

	return false;
}