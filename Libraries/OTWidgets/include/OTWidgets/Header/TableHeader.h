// @otlicense
// File: TableHeader.h
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
#include "OTWidgets/Header/HeaderBase.h"

namespace ot {

	class Table;

	class OT_WIDGETS_API_EXPORT TableHeader : public HeaderBase
	{
		OT_DECL_NOCOPY(TableHeader)
		OT_DECL_NOMOVE(TableHeader)
		OT_DECL_NODEFAULT(TableHeader)
	public:
		explicit TableHeader(Table* _table, Qt::Orientation _orientation);
		virtual ~TableHeader() = default;

	protected:
		virtual HeaderFilter::Features getFilterFeatures(int _logicalIndex) const override;
		virtual QString getFilterTitle(int _logicalIndex) const override;
		virtual QStringList getFilterOptions(int _logicalIndex) const override;

		virtual void sortOrderChangeRequest(int _logicalIndex, Qt::SortOrder _sortOrder) override;
		virtual void filterHasChanged(const HeaderFilterState& _filterState) override;

		QStringList getFilterOptionsFromText(int _logicalIndex) const;

	private:
		Table* m_table;
	};
}