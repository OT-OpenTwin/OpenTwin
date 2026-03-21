// @otlicense
// File: ProjectOverviewHeader.cpp
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
#include "ProjectOverviewWidget.h"
#include "OTWidgets/Header/HeaderFilter.h"
#include "OTWidgets/Style/IconManager.h"
#include "OTWidgets/Style/GlobalColorStyle.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

ot::ProjectOverviewHeader::ProjectOverviewHeader(ProjectOverviewWidget* _overview, QWidget* _parent)
	: HeaderBase(Orientation::Horizontal, _parent), m_overview(_overview)
{
	
}

ot::ProjectOverviewHeader::~ProjectOverviewHeader() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Overrides

bool ot::ProjectOverviewHeader::canFilter(int _logicalIndex) const
{
	return (_logicalIndex != ColumnIndex::Checked);
}

void ot::ProjectOverviewHeader::setFilterData(const ProjectFilterData& _filterData) {
    // Clear last filter to avoid mismatch in data
    m_lastFilter = ProjectOverviewFilterData();

	m_filterOptions.clear();

    // Project groups

	QStringList groupOptions;
    for (const std::string& group : _filterData.getProjectGroups()) {
        QString groupName = QString::fromStdString(group);
        groupOptions.append(std::move(groupName));
    }
    groupOptions.append(ProjectOverviewFilterData::getEmptyProjectGroupFilterName());
	m_filterOptions.emplace(ColumnIndex::Group, std::move(groupOptions));

    // Project names

	QStringList nameOptions;
    for (const std::string& name : _filterData.getProjectNames()) {
        QString nameStr = QString::fromStdString(name);
		nameOptions.append(std::move(nameStr));
	}
	m_filterOptions.emplace(ColumnIndex::Name, std::move(nameOptions));

    // Project types

    QStringList typeOptions;
    for (const std::string& type : _filterData.getProjectTypes()) {
        QString typeName = QString::fromStdString(type);
        typeOptions.append(std::move(typeName));
	}
    m_filterOptions.emplace(ColumnIndex::Type, std::move(typeOptions));

    // Project tags
	QStringList tagOptions;
    for (const std::string& tag : _filterData.getTags()) {
        QString tagName = QString::fromStdString(tag);
        tagOptions.append(std::move(tagName));
    }
    tagOptions.append(ProjectOverviewFilterData::getEmptyTagsFilterName());
    m_filterOptions.emplace(ColumnIndex::Tags, std::move(tagOptions));

	// Project owners
	QStringList ownerOptions;
    for (const std::string& owner : _filterData.getOwners()) {
        QString ownerName = QString::fromStdString(owner);
        ownerOptions.append(std::move(ownerName));
	}
    m_filterOptions.emplace(ColumnIndex::Owner, std::move(ownerOptions));

	// Project access
	QStringList accessOptions;
    for (const std::string& access : _filterData.getUserGroups()) {
        QString accessName = QString::fromStdString(access);
        accessOptions.append(std::move(accessName));
    }
    accessOptions.append(ProjectOverviewFilterData::getEmptyUserGroupFilterName());
	m_filterOptions.emplace(ColumnIndex::Access, std::move(accessOptions));
}

void ot::ProjectOverviewHeader::showFilterMenu(int _logicalIndex)
{
	if (!canFilter(_logicalIndex))
	{
		return;
	}
	QRect rect = filterIconRect(_logicalIndex);

	HeaderFilter filter(_logicalIndex, _logicalIndex == ColumnIndex::LastAccessed, m_overview->getQWidget());

	// Fill options based on column
	switch (_logicalIndex)
	{
	case ColumnIndex::Group:
		filter.setTitle("Project Group");
		break;

	case ColumnIndex::Type:
		filter.setTitle("Project Type");
		break;

	case ColumnIndex::Name:
		filter.setTitle("Project Name");
		break;

	case ColumnIndex::Tags:
		filter.setTitle("Project Tags");
		break;

	case ColumnIndex::Owner:
		filter.setTitle("Project Owner");
		break;

	case ColumnIndex::Access:
		filter.setTitle("Shared Groups");
		break;

	case ColumnIndex::LastAccessed:
		filter.setTitle("Last Accessed");
		break;

	default:
		OT_LOG_E("Invalid column for filter (" + std::to_string(_logicalIndex) + ")");
		return;
	}

	auto optionsIt = m_filterOptions.find(_logicalIndex);
	if (optionsIt != m_filterOptions.end())
	{
		filter.setOptions(optionsIt->second);
	}

	filter.updateCheckedState(m_lastFilter.getSelectedFilters());

	connect(&filter, &HeaderFilter::sortOrderChanged, this, &ProjectOverviewHeader::sortOrderChangeRequest);

	filter.showAt(mapToGlobal(rect.bottomLeft()));

	disconnect(&filter, &HeaderFilter::sortOrderChanged, this, &ProjectOverviewHeader::sortOrderChangeRequest);

	if (filter.wasConfirmed())
	{
		m_lastFilter.setSelectedFilters(filter.saveCheckedState());
		m_lastFilter.setLogicalIndex(_logicalIndex);

		if (m_lastFilter.getSelectedFilters().empty())
		{
			setActiveFilterIndex(-1);
		}
		else
		{
			setActiveFilterIndex(m_lastFilter.getLogicalIndex());
		}

		m_overview->filterProjects(m_lastFilter);
		
		update();
	}
}

void ot::ProjectOverviewHeader::sortOrderChangeRequest(int _logicalIndex, Qt::SortOrder _sortOrder)
{
	m_overview->sort(_logicalIndex, _sortOrder);
}
