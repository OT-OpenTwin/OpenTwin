// @otlicense

// OpenTwin header
#include "ProjectOverviewEntry.h"
#include "ProjectOverviewHeader.h"
#include "OTSystem/DateTime.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/IconManager.h"

ot::ProjectOverviewEntry::ProjectOverviewEntry(const ProjectInformation& _projectInfo)
	: ot::TreeWidgetItem(), m_projectInfo(_projectInfo)
{
	QDateTime modifiedTime = QDateTime::fromMSecsSinceEpoch(m_projectInfo.getLastAccessTime());

	setCheckState(ProjectOverviewHeader::Checked, Qt::Unchecked);
	setIcon(ProjectOverviewHeader::Type, ot::IconManager::getIcon("ProjectTemplates/" + QString::fromStdString(m_projectInfo.getProjectType()) + ".png"));
	setText(ProjectOverviewHeader::Name, QString::fromStdString(m_projectInfo.getProjectName()));
	setText(ProjectOverviewHeader::Owner, QString::fromStdString(m_projectInfo.getUserName()));
	setText(ProjectOverviewHeader::Modified, modifiedTime.toString(Qt::DateFormat::TextDate));
	
	if (!m_projectInfo.getGroups().empty()) {
		setIcon(ProjectOverviewHeader::Groups, ot::IconManager::getIcon("Tree/Groups.png"));
		std::string groupInfo;
		for (const std::string& group : m_projectInfo.getGroups()) {
			if (!groupInfo.empty()) groupInfo.append("\n");
			groupInfo.append(group);
		}
		setToolTip(ProjectOverviewHeader::Groups, QString::fromStdString(groupInfo));
	}

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
	case ProjectOverviewHeader::Type:
		setHidden(!_filter.getSelectedFilters().contains(QString::fromStdString(m_projectInfo.getProjectType())));
		break;

	case ProjectOverviewHeader::Name:
		setHidden(!_filter.getSelectedFilters().contains(QString::fromStdString(m_projectInfo.getProjectName())));
		break;

	case ProjectOverviewHeader::Owner:
		setHidden(!_filter.getSelectedFilters().contains(QString::fromStdString(m_projectInfo.getUserName())));
		break;

	case ProjectOverviewHeader::Groups:
	{
		bool found = false;
		if (m_projectInfo.getGroups().empty()) {
			if (_filter.getSelectedFilters().contains(ProjectOverviewFilterData::getEmptyGroupFilterName())) {
				found = true;
			}
		}
		else {
			for (const std::string& group : m_projectInfo.getGroups()) {
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