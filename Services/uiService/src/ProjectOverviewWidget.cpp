// @otlicense
// File: ProjectOverviewWidget.cpp
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
#include "AppBase.h"
#include "UserManagement.h"
#include "ProjectManagement.h"
#include "ProjectOverviewTree.h"
#include "ProjectOverviewEntry.h"
#include "ProjectOverviewHeader.h"
#include "ProjectOverviewWidget.h"
#include "ProjectOverviewDelegate.h"
#include "ProjectOverviewPreviewBox.h"
#include "OTSystem/DateTime.h"
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/TreeWidget.h"
#include "ModelState.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>

std::string ot::ProjectOverviewWidget::toString(ViewMode _mode) {
	switch (_mode) {
	case ot::ProjectOverviewWidget::ViewMode::Tree: return "Tree";
	case ot::ProjectOverviewWidget::ViewMode::List: return "List";
	default:
		OT_LOG_E("Invalid ViewMode (" + std::to_string(static_cast<int>(_mode)) + ")");
		return "Tree";
	}
}

ot::ProjectOverviewWidget::ViewMode ot::ProjectOverviewWidget::viewModeFromString(const std::string& _modeStr) {
	if (_modeStr == toString(ViewMode::Tree)) { return ViewMode::Tree; }
	else if (_modeStr == toString(ViewMode::List)) { return ViewMode::List; }
	else {
		OT_LOG_E("Invalid ViewMode \"" + _modeStr + "\"");
		return ViewMode::Tree;
	}
}

ot::ProjectOverviewWidget::ProjectOverviewWidget(QWidget* _parent)
	: QWidget(_parent), m_resultsExceeded(false), m_isLoading(false), m_viewMode(ViewMode::Tree), m_generalFilter("")
{
	// Create widgets
	QHBoxLayout* mainLayout = new QHBoxLayout(_parent);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mainLayout);

	m_tree = new ot::ProjectOverviewTree(_parent);
	m_header = new ProjectOverviewHeader(this, m_tree);
	m_tree->setHeader(m_header);

	m_tree->setObjectName("ProjectOverviewWidgetTree");
	m_tree->setIconSize(QSize(28, 28));
	m_tree->setColumnCount(ProjectOverviewHeader::ColumnIndex::Count);
	m_tree->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_tree->setSelectionMode(QAbstractItemView::SingleSelection);

	m_tree->setHeaderLabels(QStringList() << " " << "Group" << "Type" << "Name" << "Tags" << "Owner" << "Access" << "Last Modified");
	m_tree->header()->setStretchLastSection(false);
	m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_tree->header()->setSectionResizeMode(ProjectOverviewHeader::ColumnIndex::Name, QHeaderView::Stretch);
	m_tree->setRootIsDecorated(true);

	//m_tree->setItemDelegate(new ProjectOverviewDelegate);

	mainLayout->addWidget(m_tree, 1);

	m_previewBox = new ProjectOverviewPreviewBox(_parent);
	mainLayout->addWidget(m_previewBox);

	// Connect signals
	connect(m_tree, &ot::TreeWidget::itemChanged, this, &ot::ProjectOverviewWidget::slotItemChanged);
	connect(m_tree, &ot::TreeWidget::itemSelectionChanged, this, &ot::ProjectOverviewWidget::slotSelectionChanged);
	connect(m_tree, &ot::TreeWidget::itemDoubleClicked, this, &ot::ProjectOverviewWidget::slotOpenRequested);
}

ot::ProjectOverviewWidget::~ProjectOverviewWidget() {
	stopLoading();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::ProjectOverviewWidget::setViewMode(ViewMode _mode) {
	m_viewMode = _mode;
	switch (m_viewMode) {
	case ViewMode::Tree:
		{
			std::list<ProjectOverviewEntry*> entries;
			// Move all entries to their groups
			for (int i = m_tree->topLevelItemCount() - 1; i >= 0; i--) {
				ProjectOverviewEntry* entry = dynamic_cast<ProjectOverviewEntry*>(m_tree->topLevelItem(i));
				if (!entry) {
					OT_LOG_E("Invalid item found");
					continue;
				}

				entries.push_back(entry);
				m_tree->takeTopLevelItem(i);
			}

			for (ProjectOverviewEntry* entry : entries) {
				TreeWidgetItem* groupItem = getOrCreateProjectGroupItem(entry->getProjectInformation().getProjectGroup());
				groupItem->addChild(entry);
			}

			m_tree->setRootIsDecorated(true);
		}
		break;

	case ViewMode::List:
		{
			// Move all entries to the top level
			for (auto& it : m_projectGroupItems) {
				TreeWidgetItem* groupItem = it.second;
				for (int i = groupItem->childCount() - 1; i >= 0; i--) {
					ProjectOverviewEntry* entry = dynamic_cast<ProjectOverviewEntry*>(groupItem->child(i));
					if (!entry) {
						OT_LOG_E("Invalid item found");
						continue;
					}
					groupItem->takeChild(i);
					m_tree->addTopLevelItem(entry);
				}
				delete groupItem;
			}

			m_projectGroupItems.clear();
			m_tree->setRootIsDecorated(false);
		}
		break;

	default:
		OT_LOG_E("Invalid ViewMode (" + std::to_string(static_cast<int>(_mode)) + ")");
		m_tree->setRootIsDecorated(true);
		break;
	}
}

void ot::ProjectOverviewWidget::setMultiSelectionEnabled(bool _enabled) {
	m_tree->setSelectionMode(_enabled ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection);
}

bool ot::ProjectOverviewWidget::getMultiSelectionEnabled() const {
	return (m_tree->selectionMode() == QAbstractItemView::ExtendedSelection);
}

void ot::ProjectOverviewWidget::setGeneralFilter(const QString& _filter) {
	m_generalFilter = _filter;
	slotBasicFilterProjects();
}

int ot::ProjectOverviewWidget::getProjectCount() const {
	return getProjectCount(m_tree->invisibleRootItem());
}

std::list<ot::ProjectInformation> ot::ProjectOverviewWidget::getAllProjects() const {
	std::list<ProjectInformation> result;
	getAllProjects(m_tree->invisibleRootItem(), result);
	return result;
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
	m_projectGroupItems.clear();
	m_tree->clear();
}

void ot::ProjectOverviewWidget::refreshProjects() {
	// Store expanded project groups
	std::set<std::string> expandedProjectGroups;
	for (auto& it : m_projectGroupItems) {
		TreeWidgetItem* projectGroup = it.second;
		if (projectGroup->isExpanded()) {
			expandedProjectGroups.insert(it.first);
		}
	}

	clear();

	m_resultsExceeded = false;

	const AppBase* app = AppBase::instance();
	
	std::list<ot::ProjectInformation> projects;
	ProjectManagement projectManager(app->getCurrentLoginData());
	projectManager.findProjects(m_generalFilter.toStdString(), 0, projects, m_resultsExceeded);

	for (const ot::ProjectInformation& proj : projects) {
		std::string editorName("< Unknown >");
		addEntry(new ProjectOverviewEntry(proj));
	}

	// Restore expanded project groups
	if (!expandedProjectGroups.empty()) {
		for (auto& it : m_projectGroupItems) {
			TreeWidgetItem* groupItem = it.second;
			if (expandedProjectGroups.find(it.first) != expandedProjectGroups.end()) {
				groupItem->setExpanded(true);
			}
			else {
				groupItem->setExpanded(false);
			}
		}
	}
	
	updateProjectGroups();

	sort(m_header->getLastFilter());
	m_header->resetLastFilter();
}

void ot::ProjectOverviewWidget::filterProjects(const ProjectOverviewFilterData& _filterData) {
	filterProjects(m_tree->invisibleRootItem(), _filterData);
	updateProjectGroups();
	
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::ProjectOverviewWidget::slotSelectionChanged() {
	QSignalBlocker sigBlock(m_tree);

	updateCheckStates(m_tree->invisibleRootItem());
	
	if (m_tree->selectedItems().size() == 1) {
		const ProjectOverviewEntry* entry = dynamic_cast<const ProjectOverviewEntry*>(m_tree->selectedItems().front());
		if (entry) {
			startLoading(entry->getProjectInformation());
		}
		else {
			m_previewBox->unsetProject();
			stopLoading();
		}
	}
	else {
		m_previewBox->unsetProject();
		stopLoading();
	}

	Q_EMIT selectionChanged();
}

void ot::ProjectOverviewWidget::slotItemChanged(QTreeWidgetItem* _item, int _column) {
	if (_column != ProjectOverviewHeader::ColumnIndex::Checked) {
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
	if (_column == ProjectOverviewHeader::ColumnIndex::Checked) {
		return;
	}

	ProjectOverviewEntry* entry = dynamic_cast<ProjectOverviewEntry*>(_item);
	if (!entry) {
		return;
	}
	m_previewBox->unsetProject();

	Q_EMIT projectOpenRequested(entry->getProjectInformation());
}

void ot::ProjectOverviewWidget::slotBasicFilterProjects() {
	basicFilterProjects(m_tree->invisibleRootItem());
	updateProjectGroups();
}

void ot::ProjectOverviewWidget::slotWorkerFinished() {
	m_previewBox->setProject(m_importedProjectData);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::ProjectOverviewWidget::startLoading(const ProjectInformation& _projectInfo) {
	stopLoading();

	m_isLoading = true;
	m_currentlyLoadingProject = _projectInfo;
	m_loadingThread.reset(new std::thread(&ot::ProjectOverviewWidget::worker, this));
}

void ot::ProjectOverviewWidget::stopLoading() {
	m_isLoading = false;
	if (m_loadingThread.get()) {
		m_loadingThread->join();
		m_loadingThread.reset();
	}
}

void ot::ProjectOverviewWidget::worker() {
	if (!m_isLoading) {
		return;
	}
	// Reset
	m_importedProjectData = ExtendedProjectInformation(m_currentlyLoadingProject);

	// Load preview icon
	std::vector<char> imageData;
	ot::ImageFileFormat imageFormat = ot::ImageFileFormat::PNG;
	
	if (ModelState::readProjectPreviewImage(m_currentlyLoadingProject.getCollectionName(), imageData, imageFormat)) {
		m_importedProjectData.setImageData(std::move(imageData), imageFormat);
	}

	std::string description;
	ot::DocumentSyntax syntax = ot::DocumentSyntax::PlainText;
	if (ModelState::readProjectDescription(m_currentlyLoadingProject.getCollectionName(), description, syntax)) {
		m_importedProjectData.setDescription(description);
		m_importedProjectData.setDescriptionSyntax(syntax);
	}

		QMetaObject::invokeMethod(this, &ProjectOverviewWidget::slotWorkerFinished, Qt::QueuedConnection);
}

void ot::ProjectOverviewWidget::addEntry(ProjectOverviewEntry* _entry) {
	for (int i = 0; i < ProjectOverviewHeader::Count; i++) {
		_entry->setData(i, Qt::UserRole, false);
	}

	if (m_viewMode == ViewMode::List) {
		m_tree->addTopLevelItem(_entry);
	}
	else {
		TreeWidgetItem* groupItem = getOrCreateProjectGroupItem(_entry->getProjectInformation().getProjectGroup());
		groupItem->addChild(_entry);
	}
}

void ot::ProjectOverviewWidget::updateProjectGroups() {
	for (auto& it : m_projectGroupItems) {
		updateProjectGroup(it.second);
	}
}

void ot::ProjectOverviewWidget::updateProjectGroup(TreeWidgetItem* _groupItem) {
	int visibleItems = 0;
	for (int i = 0; i < _groupItem->childCount(); i++) {
		if (!_groupItem->child(i)->isHidden()) {
			visibleItems++;
		}
	}

	_groupItem->setHidden(visibleItems == 0);
}

void ot::ProjectOverviewWidget::updateCheckStates(QTreeWidgetItem* _parent) {
	for (int i = 0; i < _parent->childCount(); i++) {
		updateCheckStates(_parent->child(i));
		ProjectOverviewEntry* entry = dynamic_cast<ProjectOverviewEntry*>(_parent->child(i));
		if (entry) {
			entry->updateCheckState();
		}
	}
}

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

void ot::ProjectOverviewWidget::getAllProjects(const QTreeWidgetItem* _parent, std::list<ProjectInformation>& _lst) const {
	for (int i = 0; i < _parent->childCount(); i++) {
		getAllProjects(_parent->child(i), _lst);
		const ProjectOverviewEntry* entry = dynamic_cast<const ProjectOverviewEntry*>(_parent->child(i));
		if (entry) {
			_lst.push_back(entry->getProjectInformation());
		}
	}
}

void ot::ProjectOverviewWidget::basicFilterProjects(QTreeWidgetItem* _parent) {
	for (int i = 0; i < _parent->childCount(); i++) {
		basicFilterProjects(_parent->child(i));
		ProjectOverviewEntry* entry = dynamic_cast<ProjectOverviewEntry*>(_parent->child(i));
		if (entry) {
			entry->applyFilter(m_generalFilter);
		}
	}
}

void ot::ProjectOverviewWidget::filterProjects(const QTreeWidgetItem* _parent, const ProjectOverviewFilterData& _filterData) {
	for (int i = 0; i < _parent->childCount(); i++) {
		filterProjects(_parent->child(i), _filterData);
		ProjectOverviewEntry* entry = dynamic_cast<ProjectOverviewEntry*>(_parent->child(i));
		if (entry) {
			entry->applyFilter(_filterData);
		}
	}

	sort(_filterData);
}

void ot::ProjectOverviewWidget::sort(const ProjectOverviewFilterData& _filterData) {
	if (_filterData.getLogicalIndex() >= 0 && _filterData.getSortMode() != ProjectOverviewFilterData::SortMode::None) {
		Qt::SortOrder sortOrder = (_filterData.getSortMode() == ProjectOverviewFilterData::SortMode::Ascending) ? Qt::AscendingOrder : Qt::DescendingOrder;
		m_tree->sortByColumn(_filterData.getLogicalIndex(), sortOrder);
	}
}

ot::TreeWidgetItem* ot::ProjectOverviewWidget::getOrCreateProjectGroupItem(const std::string& _groupName) {
	auto it = m_projectGroupItems.find(_groupName);
	if (it != m_projectGroupItems.end()) {
		return it->second;
	}

	TreeWidgetItem* projectGroupItem = new TreeWidgetItem;

	QString groupText = QString::fromStdString(_groupName);
	if (groupText.isEmpty()) {
		groupText = "< Ungrouped >";
	}

	projectGroupItem->setData(ProjectOverviewHeader::Checked, Qt::UserRole + 10, groupText);
	QFont font = projectGroupItem->font(ProjectOverviewHeader::Checked);
	font.setBold(true);
	font.setPointSize(font.pointSize() + 12);
	projectGroupItem->setFont(ProjectOverviewHeader::Checked, font);
	projectGroupItem->setFlags(projectGroupItem->flags() & ~Qt::ItemIsSelectable);
	m_tree->addTopLevelItem(projectGroupItem);
	projectGroupItem->setExpanded(true);

	for (int i = 0; i < ProjectOverviewHeader::Count; i++) {
		projectGroupItem->setData(i, Qt::UserRole, true);
	}

	m_projectGroupItems.emplace(_groupName, projectGroupItem);

	return projectGroupItem;
}
