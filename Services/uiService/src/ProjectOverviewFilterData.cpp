// @otlicense
// File: ProjectOverviewFilterData.cpp
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
#include "ProjectOverviewHeader.h"
#include "ProjectOverviewFilterData.h"
#include "OTCore/LogDispatcher.h"

ot::ProjectOverviewFilterData::ProjectOverviewFilterData(int _logicalIndex)
	: m_logicalIndex(_logicalIndex)
{

}

ot::ProjectFilterData ot::ProjectOverviewFilterData::toProjectFilterData() const {
	ProjectFilterData filter;

	std::list<std::string> options;
	for (const QString& option : m_selectedFilters) {
		if (option == getEmptyProjectGroupFilterName() ||
			option == getEmptyTagsFilterName() ||
			option == getEmptyUserGroupFilterName()) 
		{
			options.push_back("");
		}
		else {
			options.push_back(option.toStdString());
		}
		
	}
	switch (m_logicalIndex) {
	case ProjectOverviewHeader::Group:
		filter.setProjectGroups(std::move(options));
		break;

	case ProjectOverviewHeader::Type:
		filter.setProjectTypes(std::move(options));
		break;

	case ProjectOverviewHeader::Name:
		filter.setProjectNames(std::move(options));
		break;

	case ProjectOverviewHeader::Tags:
		filter.setTags(std::move(options));
		break;

	case ProjectOverviewHeader::Owner:
		filter.setOwners(std::move(options));
		break;

	case ProjectOverviewHeader::Access:
		filter.setUserGroups(std::move(options));
		break;

	default:
		break;
	}

	return filter;
}
