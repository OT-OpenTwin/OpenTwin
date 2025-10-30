// @otlicense
// File: ProjectOverviewEntry.cpp
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
#include "ProjectOverviewEntry.h"
#include "ProjectOverviewHeader.h"
#include "OTSystem/DateTime.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/IconManager.h"

ot::ProjectOverviewEntry::ProjectOverviewEntry(const ProjectInformation& _projectInfo)
	: ot::TreeWidgetItem(), m_projectInfo(_projectInfo)
{
	QDateTime accessedTime = QDateTime::fromMSecsSinceEpoch(m_projectInfo.getLastAccessTime());

	setCheckState(ProjectOverviewHeader::Checked, Qt::Unchecked);
	setText(ProjectOverviewHeader::Group, QString::fromStdString(m_projectInfo.getProjectGroup()));
	setText(ProjectOverviewHeader::Name, QString::fromStdString(m_projectInfo.getProjectName()));
	setText(ProjectOverviewHeader::Owner, QString::fromStdString(m_projectInfo.getUserName()));
	setText(ProjectOverviewHeader::LastAccessed, accessedTime.toString(Qt::DateFormat::TextDate));
	
	setIcon(ProjectOverviewHeader::Type, ot::IconManager::getIcon("ProjectTemplates/" + QString::fromStdString(m_projectInfo.getProjectType()) + ".png"));
	setToolTip(ProjectOverviewHeader::Type, QString::fromStdString(m_projectInfo.getProjectType()));

	if (!m_projectInfo.getTags().empty()) {
		setIcon(ProjectOverviewHeader::Tags, ot::IconManager::getIcon("Tree/Tag.png"));
		std::string tagInfo;
		for (const std::string& tag : m_projectInfo.getTags()) {
			if (!tagInfo.empty()) tagInfo.append("\n");
			tagInfo.append(tag);
		}
		setToolTip(ProjectOverviewHeader::Tags, QString::fromStdString(tagInfo));
	}

	if (!m_projectInfo.getUserGroups().empty()) {
		setIcon(ProjectOverviewHeader::Access, ot::IconManager::getIcon("Tree/Groups.png"));
		std::string groupInfo;
		for (const std::string& group : m_projectInfo.getUserGroups()) {
			if (!groupInfo.empty()) groupInfo.append("\n");
			groupInfo.append(group);
		}
		setToolTip(ProjectOverviewHeader::Access, QString::fromStdString(groupInfo));
	}

	setData(ProjectOverviewHeader::LastAccessed, Qt::UserRole + 20, m_projectInfo.getLastAccessTime());
}

void ot::ProjectOverviewEntry::applyFilter(const QString& _generalFilter) {
	if (_generalFilter.isEmpty()) {
		setHidden(false);
		return;
	}
	QString filter = _generalFilter.toLower();
	bool matches = text(ProjectOverviewHeader::Name).toLower().contains(filter);
	
	setHidden(!matches);
}

void ot::ProjectOverviewEntry::applyFilter(const ProjectOverviewFilterData& _filter) {
	if (!_filter.isValid()) {
		setHidden(false);
		return;
	}

	switch (_filter.getLogicalIndex()) {
	case ProjectOverviewHeader::Group:
		if (m_projectInfo.getProjectGroup().empty()) {
			setHidden(!_filter.getSelectedFilters().contains(ProjectOverviewFilterData::getEmptyProjectGroupFilterName()));
		}
		else {
			setHidden(!_filter.getSelectedFilters().contains(QString::fromStdString(m_projectInfo.getProjectGroup())));
		}
		break;

	case ProjectOverviewHeader::Type:
		setHidden(!_filter.getSelectedFilters().contains(QString::fromStdString(m_projectInfo.getProjectType())));
		break;

	case ProjectOverviewHeader::Name:
		setHidden(!_filter.getSelectedFilters().contains(QString::fromStdString(m_projectInfo.getProjectName())));
		break;

	case ProjectOverviewHeader::Owner:
		setHidden(!_filter.getSelectedFilters().contains(QString::fromStdString(m_projectInfo.getUserName())));
		break;

	case ProjectOverviewHeader::Tags:
	{
		bool found = false;
		if (m_projectInfo.getTags().empty()) {
			if (_filter.getSelectedFilters().contains(ProjectOverviewFilterData::getEmptyTagsFilterName())) {
				found = true;
			}
		}
		else {
			for (const std::string& tag : m_projectInfo.getTags()) {
				if (_filter.getSelectedFilters().contains(QString::fromStdString(tag))) {
					found = true;
					break;
				}
			}
		}
		setHidden(!found);
		break;
	}

	case ProjectOverviewHeader::Access:
	{
		bool found = false;
		if (m_projectInfo.getUserGroups().empty()) {
			if (_filter.getSelectedFilters().contains(ProjectOverviewFilterData::getEmptyUserGroupFilterName())) {
				found = true;
			}
		}
		else {
			for (const std::string& group : m_projectInfo.getUserGroups()) {
				if (_filter.getSelectedFilters().contains(QString::fromStdString(group))) {
					found = true;
					break;
				}
			}
		}
		setHidden(!found);
		break;
	}

	default:
		OT_LOG_E("Invalid column for filter (" + std::to_string(_filter.getLogicalIndex()) + ")");
		setHidden(false);
		break;


	}

}

bool ot::ProjectOverviewEntry::operator<(const QTreeWidgetItem& _other) const {
	int column = treeWidget()->sortColumn();
	if (column == ProjectOverviewHeader::LastAccessed) {
		return data(column, Qt::UserRole + 20).toLongLong() < _other.data(column, Qt::UserRole + 20).toLongLong();
	}
	else {
		return TreeWidgetItem::operator<(_other);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

void ot::ProjectOverviewEntry::updateSelectionState() {
	bool shouldSelect = (checkState(ProjectOverviewHeader::Checked) == Qt::Checked);
	if (shouldSelect == isSelected()) {
		return;
	}

	setSelected(shouldSelect);
}

void ot::ProjectOverviewEntry::updateCheckState() {
	bool shouldCheck = isSelected();
	if (shouldCheck == (checkState(ProjectOverviewHeader::Checked) == Qt::Checked)) {
		return;
	}

	setCheckState(ProjectOverviewHeader::Checked, shouldCheck ? Qt::Checked : Qt::Unchecked);
}