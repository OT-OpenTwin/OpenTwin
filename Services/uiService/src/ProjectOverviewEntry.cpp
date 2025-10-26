// @otlicense

// OpenTwin header
#include "ProjectOverviewEntry.h"
#include "OTSystem/DateTime.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/IconManager.h"

ot::ProjectOverviewEntry::ProjectOverviewEntry(const ProjectInformation& _projectInfo)
	: ot::TreeWidgetItem(), m_projectInfo(_projectInfo)
{
	QDateTime modifiedTime = QDateTime::fromMSecsSinceEpoch(m_projectInfo.getLastAccessTime());

	setCheckState(ColumnIndex::Checked, Qt::Unchecked);
	setIcon(ColumnIndex::Type, ot::IconManager::getIcon("ProjectTemplates/" + QString::fromStdString(m_projectInfo.getProjectType()) + ".png"));
	setText(ColumnIndex::Name, QString::fromStdString(m_projectInfo.getProjectName()));
	setText(ColumnIndex::Owner, QString::fromStdString(m_projectInfo.getUserName()));
	setText(ColumnIndex::Modified, modifiedTime.toString(Qt::DateFormat::TextDate));
	
	if (!m_projectInfo.getGroups().empty()) {
		setIcon(ColumnIndex::Groups, ot::IconManager::getIcon("Tree/Groups.png"));
		std::string groupInfo;
		for (const std::string& group : m_projectInfo.getGroups()) {
			if (!groupInfo.empty()) groupInfo.append("\n");
			groupInfo.append(group);
		}
		setToolTip(ColumnIndex::Groups, QString::fromStdString(groupInfo));
	}

}

void ot::ProjectOverviewEntry::applyFilter(const QString& _generalFilter) {
	if (_generalFilter.isEmpty()) {
		setHidden(false);
		return;
	}
	QString filter = _generalFilter.toLower();
	bool matches = text(ColumnIndex::Name).toLower().contains(filter) || text(ColumnIndex::Owner).toLower().contains(filter);
	
	setHidden(!matches);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

void ot::ProjectOverviewEntry::updateSelectionState() {
	bool shouldSelect = (checkState(ColumnIndex::Checked) == Qt::Checked);
	if (shouldSelect == isSelected()) {
		return;
	}

	setSelected(shouldSelect);
}

void ot::ProjectOverviewEntry::updateCheckState() {
	bool shouldCheck = isSelected();
	if (shouldCheck == (checkState(ColumnIndex::Checked) == Qt::Checked)) {
		return;
	}

	setCheckState(ColumnIndex::Checked, shouldCheck ? Qt::Checked : Qt::Unchecked);
}