// @otlicense
// File: HeaderFilterState.h
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
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

// std header
#include <map>
#include <list>

namespace ot {

	class OT_WIDGETS_API_EXPORT HeaderFilterState
	{
		OT_DECL_DEFCOPY(HeaderFilterState)
		OT_DECL_DEFMOVE(HeaderFilterState)
	public:
		HeaderFilterState() = default;
		virtual ~HeaderFilterState() = default;

		void clear();

		inline void setHoveredIndex(int _index) { m_hoveredFilter = _index; };
		inline int getHoveredIndex() const { return m_hoveredFilter; };

		inline void setPressedIndex(int _index) { m_pressedFilter = _index; };
		inline int getPressedIndex() const { return m_pressedFilter; };

		void setFilter(int _index, const QStringList& _filters) { m_selectedFilters.insert_or_assign(_index, _filters); };
		QStringList getFilter(int _index) const;
		std::map<int, QStringList> getActiveFilters() const;

		bool hasActiveFilter() const;
		bool isFilterActive(int _index) const;

	private:
		int m_hoveredFilter = -1;
		int m_pressedFilter = -1;
		std::map<int, QStringList> m_selectedFilters;
	};

}