// OpenTwin header
#include "AppBase.h"
#include "UserManagement.h"
#include "ProjectManagement.h"
#include "ProjectOverviewEntry.h"
#include "ProjectOverviewHeader.h"
#include "ProjectOverviewWidget.h"
#include "OTSystem/DateTime.h"
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/TreeWidget.h"

// Qt header
#include <QtWidgets/qheaderview.h>

ot::ProjectOverviewWidget::ProjectOverviewWidget(QWidget* _parent) 
	: m_mode(RecentMode), m_resultsExceeded(false)
{
	// Create widgets
	m_tree = new ot::TreeWidget(_parent);
	m_header = new ProjectOverviewHeader(m_tree);
	m_tree->setHeader(m_header);

	m_tree->setObjectName("ProjectOverviewWidgetTree");
	m_tree->setIconSize(QSize(28, 28));
	m_tree->setColumnCount(ProjectOverviewEntry::ColumnIndex::Count);
	m_tree->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_tree->setSelectionMode(QAbstractItemView::SingleSelection);

	m_tree->setHeaderLabels(QStringList() << " " << "Type" << "Name" << "Owner" << "Groups" << "Last Modified");
	m_tree->header()->setStretchLastSection(false);
	m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_tree->header()->setSectionResizeMode(ProjectOverviewEntry::ColumnIndex::Name, QHeaderView::Stretch);
	
	// Connect signals
	connect(m_tree, &ot::TreeWidget::itemChanged, this, &ot::ProjectOverviewWidget::slotRefreshSelection);
	connect(m_tree, &ot::TreeWidget::itemSelectionChanged, this, &ot::ProjectOverviewWidget::slotRefreshAllSelection);
	connect(m_tree, &ot::TreeWidget::itemDoubleClicked, this, &ot::ProjectOverviewWidget::slotOpenRequested);
}

ot::ProjectOverviewWidget::~ProjectOverviewWidget() {
	delete m_tree;
}

QWidget* ot::ProjectOverviewWidget::getQWidget() { return m_tree; }

const QWidget* ot::ProjectOverviewWidget::getQWidget() const { return m_tree; }

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::ProjectOverviewWidget::setMultiSelectionEnabled(bool _enabled) {
	m_tree->setSelectionMode(_enabled ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection);
}

bool ot::ProjectOverviewWidget::getMultiSelectionEnabled() const {
	return (m_tree->selectionMode() == QAbstractItemView::ExtendedSelection);
}

void ot::ProjectOverviewWidget::setGeneralFilter(const QString& _filter) {
	m_generalFilter = _filter;
	slotFilterProjects();
}

int ot::ProjectOverviewWidget::getProjectCount() const {
	return getProjectCount(m_tree->invisibleRootItem());
}

bool ot::ProjectOverviewWidget::hasOtherUser(const std::string& _currentUser) const {
	return hasOtherUser(m_tree->invisibleRootItem(), _currentUser);
}

std::list<ot::ProjectInformation> ot::ProjectOverviewWidget::getSelectedProjects() const {
	std::list<ProjectInformation> lst;

	for (const QTreeWidgetItem* item : m_tree->selectedItems()) {
		const ProjectOverviewEntry* entry = dynamic_cast<const ProjectOverviewEntry*>(item);
		if (entry) {
			lst.push_back(entry->getProjectInformation());
		}
	}

	return lst;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

void ot::ProjectOverviewWidget::clear() {
	m_tree->clear();
}

void ot::ProjectOverviewWidget::refreshProjectList() {
	switch (m_mode) {
	case ot::ProjectOverviewWidget::RecentMode:
		refreshRecentProjects();
		break;

	case ot::ProjectOverviewWidget::AllMode:
		refreshAllProjects();
		break;

	default:
		OT_LOG_E("Unknown data mode (" + std::to_string(static_cast<int64_t>(m_mode)));
		break;
	}
}

void ot::ProjectOverviewWidget::refreshRecentProjects() {
	clear();

	m_mode = RecentMode;
	m_resultsExceeded = false;

	const AppBase* app = AppBase::instance();
	
	std::list<std::string> projects;
	UserManagement userManager(app->getCurrentLoginData());
	ProjectManagement projectManager(app->getCurrentLoginData());

	std::list<std::string> recent;
	userManager.getListOfRecentProjects(recent);
	if (!projectManager.readProjectsInfo(recent)) {
		OT_LOG_E("Read project author failed");
		return;
	}

	for (const std::string& proj : recent) {
		std::string editorName("< Unknown >");
		ot::ProjectInformation newInfo = projectManager.getProjectInformation(proj);

		if (newInfo.getProjectName().empty()) {
			OT_LOG_E("Project information for project \"" + proj + "\" not found");
		}
		else {
			m_tree->addTopLevelItem(new ProjectOverviewEntry(newInfo));
		}
	}
}

void ot::ProjectOverviewWidget::refreshAllProjects() {
	clear();

	m_mode = AllMode;
	m_resultsExceeded = false;

	const AppBase* app = AppBase::instance();
	
	std::list<ot::ProjectInformation> projects;
	ProjectManagement projectManager(app->getCurrentLoginData());
	projectManager.findProjects(m_generalFilter.toStdString(), 100, projects, m_resultsExceeded);

	for (const ot::ProjectInformation& proj : projects) {
		std::string editorName("< Unknown >");
		//projectManager.getProjectAuthor(proj.getProjectName(), editorName);
		m_tree->addTopLevelItem(new ProjectOverviewEntry(proj));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::ProjectOverviewWidget::slotRefreshAllSelection() {
	QSignalBlocker sigBlock(m_tree);

	for (int ix = 0; ix < m_tree->topLevelItemCount(); ix++) {
		ProjectOverviewEntry* entry = dynamic_cast<ProjectOverviewEntry*>(m_tree->topLevelItem(ix));
		if (entry) {
			entry->updateCheckState();
		}
	}

	Q_EMIT selectionChanged();
}

void ot::ProjectOverviewWidget::slotRefreshSelection(QTreeWidgetItem* _item, int _column) {
	if (_column != ProjectOverviewEntry::ColumnIndex::Checked) {
		return;
	}

	QSignalBlocker sigBlock(m_tree);

	ProjectOverviewEntry* entry = dynamic_cast<ProjectOverviewEntry*>(_item);
	if (!entry) {
		return;
	}

	entry->updateSelectionState();

	Q_EMIT selectionChanged();
}

void ot::ProjectOverviewWidget::slotOpenRequested(QTreeWidgetItem* _item, int _column) {
	if (_column == ProjectOverviewEntry::ColumnIndex::Checked) {
		return;
	}

	const ProjectOverviewEntry* entry = dynamic_cast<const ProjectOverviewEntry*>(_item);
	if (!entry) {
		return;
	}

	Q_EMIT projectOpenRequested(entry->getProjectInformation());
}

void ot::ProjectOverviewWidget::slotFilterProjects() {
	filterProjects(m_tree->invisibleRootItem());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

int ot::ProjectOverviewWidget::getProjectCount(const QTreeWidgetItem* _parent) const {
	int count = 0;
	for (int i = 0; i < _parent->childCount(); i++) {
		count += getProjectCount(_parent->child(i));
		const ProjectOverviewEntry* entry = dynamic_cast<const ProjectOverviewEntry*>(_parent->child(i));
		if (entry) {
			count++;
		}
	}
	return count;
}

bool ot::ProjectOverviewWidget::hasOtherUser(const QTreeWidgetItem* _parent, const std::string& _currentUser) const {
	for (int i = 0; i < _parent->childCount(); i++) {
		if (hasOtherUser(_parent->child(i), _currentUser)) {
			return true;
		}
		const ProjectOverviewEntry* entry = dynamic_cast<const ProjectOverviewEntry*>(_parent->child(i));
		if (entry) {
			if (entry->getProjectInformation().getUserName() != _currentUser) {
				return true;
			}
		}
	}

	return false;
}

void ot::ProjectOverviewWidget::filterProjects(QTreeWidgetItem* _parent) {
	for (int i = 0; i < _parent->childCount(); i++) {
		filterProjects(_parent->child(i));
		ProjectOverviewEntry* entry = dynamic_cast<ProjectOverviewEntry*>(_parent->child(i));
		if (entry) {
			entry->applyFilter(m_generalFilter);
		}
	}
}
