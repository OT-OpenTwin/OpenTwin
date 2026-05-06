// @otlicense
// File: ProjectOverviewHeader.h
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
#include "OTWidgets/Header/HeaderBase.h"

// std header
#include <map>

namespace ot {

	class ProjectOverviewWidget;

	class ProjectOverviewHeader : public HeaderBase
	{
		Q_OBJECT
		OT_DECL_NOCOPY(ProjectOverviewHeader)
		OT_DECL_NOMOVE(ProjectOverviewHeader)
		OT_DECL_NODEFAULT(ProjectOverviewHeader)
	public:
		enum ColumnIndex {
			Checked,
			Group,
			Type,
			Name,
			Tags,
			Owner,
			Access,
			LastAccessed,
			Count
		};

		explicit ProjectOverviewHeader(ProjectOverviewWidget* _overview, QWidget* _parent);
		virtual ~ProjectOverviewHeader();

		void setProjectFilterData(const ProjectFilterData& _filterData);

		ProjectFilterData getProjectFilterData() const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Overrides

	protected:
		virtual ot::HeaderFilter::Features getFilterFeatures(int _logicalIndex) const override;
		virtual QString getFilterTitle(int _logicalIndex) const override;
		virtual QStringList getFilterOptions(int _logicalIndex) const override;

		virtual void sortOrderChangeRequest(int _logicalIndex, Qt::SortOrder _sortOrder) override;
		virtual void filterHasChanged(const ot::HeaderFilterState& _filterState) override;

		static QString getEmptyProjectGroupFilterName() { return "< No Group >"; };
		static QString getEmptyUserGroupFilterName() { return "< No Groups >"; };
		static QString getEmptyTagsFilterName() { return "< No Tags >"; };

	private:
		ProjectOverviewWidget* m_overview;

		std::map<int, QStringList> m_filterOptions;		
	};

}