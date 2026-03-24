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

ot::HeaderFilter::Features ot::ProjectOverviewHeader::getFilterFeatures(int _logicalIndex) const
{
	HeaderFilter::Features features;

	if (_logicalIndex != ProjectOverviewHeader::Checked)
	{
		features.set(ot::HeaderFilter::Feature::SortingEnabled);

		if (_logicalIndex != ProjectOverviewHeader::LastAccessed)
		{
			features.set(ot::HeaderFilter::Feature::FilteringEnabled);
		}
	}

	return features;
}

QString ot::ProjectOverviewHeader::getFilterTitle(int _logicalIndex) const
{
	switch (_logicalIndex)
	{
	case ColumnIndex::Group: return "Project Group";
	case ColumnIndex::Type: return "Project Type";
	case ColumnIndex::Name: return "Project Name";
	case ColumnIndex::Tags: return "Project Tags";
	case ColumnIndex::Owner: return "Project Owner";
	case ColumnIndex::Access: return "Shared Groups";
	case ColumnIndex::LastAccessed: return "Last Accessed";
	default:
		OT_LOG_E("Invalid column for filter (" + std::to_string(_logicalIndex) + ")");
		return "";
	}
}

QStringList ot::ProjectOverviewHeader::getFilterOptions(int _logicalIndex) const
{
	const auto optionsIt = m_filterOptions.find(_logicalIndex);
	if (optionsIt != m_filterOptions.end())
	{
		return optionsIt->second;
	}
	else
	{
		return QStringList();
	}
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

void ot::ProjectOverviewHeader::sortOrderChangeRequest(int _logicalIndex, Qt::SortOrder _sortOrder)
{
	m_overview->sort(_logicalIndex, _sortOrder);
}

void ot::ProjectOverviewHeader::filterOptionsChanged(int _logicalIndex, const QStringList& _selectedOptions)
{
	m_lastFilter.setSelectedFilters(_selectedOptions);
	m_lastFilter.setLogicalIndex(_logicalIndex);
	m_overview->filterProjects(m_lastFilter);
}
