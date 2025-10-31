// @otlicense
// File: ProjectOverviewFilterData.h
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
#include "OTCore/ProjectFilterData.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

namespace ot {

	class ProjectOverviewFilterData {
		OT_DECL_DEFCOPY(ProjectOverviewFilterData)	
		OT_DECL_DEFMOVE(ProjectOverviewFilterData)
	public:
		enum SortMode {
			Ascending,
			Descending,
			None
		};

		ProjectOverviewFilterData(int _logicalIndex = -1);
		~ProjectOverviewFilterData() = default;

		bool isValid() const { return !m_selectedFilters.empty() && m_logicalIndex >= 0; };

		void setLogicalIndex(int _logicalIndex) { m_logicalIndex = _logicalIndex; };
		int getLogicalIndex() const { return m_logicalIndex; };

		void addSelectedFilter(const QString& _filter) { m_selectedFilters.append(_filter); };
		void setSelectedFilters(const QStringList& _filters) { m_selectedFilters = _filters; };
		const QStringList& getSelectedFilters() const { return m_selectedFilters; };

		ProjectFilterData toProjectFilterData() const;

		static QString getEmptyProjectGroupFilterName() { return "< No Group >"; };
		static QString getEmptyUserGroupFilterName() { return "< No Groups >"; };
		static QString getEmptyTagsFilterName() { return "< No Tags >"; };

	private:
		QStringList m_selectedFilters;
		int m_logicalIndex;
	};

}