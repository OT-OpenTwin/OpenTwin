// @otlicense

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

ot::ProjectOverviewWidget::ProjectOverviewWidget(QWidget* _parent) 
	: m_mode(RecentMode), m_resultsExceeded(false), m_isLoading(false)
{
	// Create widgets
	QHBoxLayout* mainLayout = new QHBoxLayout(_parent);
	setLayout(mainLayout);

	m_tree = new ot::ProjectOverviewTree(_parent);
	m_header = new ProjectOverviewHeader(this, m_tree);
	m_tree->setHeader(m_header);

	m_tree->setObjectName("ProjectOverviewWidgetTree");
	m_tree->setIconSize(QSize(28, 28));
	m_tree->setColumnCount(ProjectOverviewHeader::ColumnIndex::Count);
	m_tree->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_tree->setSelectionMode(QAbstractItemView::SingleSelection);

	m_tree->setHeaderLabels(QStringList() << " " << "Type" << "Name" << "Tags" << "Owner" << "Groups" << "Last Modified");
	m_tree->header()->setStretchLastSection(false);
	m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_tree->header()->setSectionResizeMode(ProjectOverviewHeader::ColumnIndex::Name, QHeaderView::Stretch);

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
	m_categoryItems.clear();
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
			addEntry(new ProjectOverviewEntry(newInfo));
		}
	}

	updateCategories();
	m_tree->sortByColumn(ProjectOverviewHeader::ColumnIndex::Modified, Qt::DescendingOrder);
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
		addEntry(new ProjectOverviewEntry(proj));
	}

	updateCategories();
	m_tree->sortByColumn(ProjectOverviewHeader::ColumnIndex::Modified, Qt::DescendingOrder);
}

void ot::ProjectOverviewWidget::filterProjects(const ProjectOverviewFilterData& _filterData) {
	filterProjects(m_tree->invisibleRootItem(), _filterData);
	updateCategories();
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

	const ProjectOverviewEntry* entry = dynamic_cast<const ProjectOverviewEntry*>(_item);
	if (!entry) {
		return;
	}

	Q_EMIT projectOpenRequested(entry->getProjectInformation());
}

void ot::ProjectOverviewWidget::slotBasicFilterProjects() {
	basicFilterProjects(m_tree->invisibleRootItem());
	updateCategories();
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

	auto it = m_categoryItems.find(_entry->getProjectInformation().getCategory());
	if (it != m_categoryItems.end()) {
		it->second->addChild(_entry);
		return;
	}
	
	TreeWidgetItem* categoryItem = new TreeWidgetItem;

	QString categoryText = QString::fromStdString(_entry->getProjectInformation().getCategory());
	if (categoryText.isEmpty()) {
		categoryText = "< Uncategorized >";
	}

	categoryItem->setData(ProjectOverviewHeader::Checked, Qt::UserRole + 10, categoryText);
	QFont font = categoryItem->font(ProjectOverviewHeader::Checked);
	font.setBold(true);
	font.setPointSize(font.pointSize() + 12);
	categoryItem->setFont(ProjectOverviewHeader::Checked, font);
	categoryItem->setFlags(categoryItem->flags() & ~Qt::ItemIsSelectable);
	m_tree->addTopLevelItem(categoryItem);
	categoryItem->addChild(_entry);
	categoryItem->setExpanded(true);

	for (int i = 0; i < ProjectOverviewHeader::Count; i++) {
		categoryItem->setData(i, Qt::UserRole, true);
	}

	m_categoryItems.emplace(_entry->getProjectInformation().getCategory(), categoryItem);
}

void ot::ProjectOverviewWidget::updateCategories() {
	// Move items to categories
	for (int i = m_tree->topLevelItemCount() - 1; i >= 0; i--) {
		QTreeWidgetItem* topItem = m_tree->topLevelItem(i);
		ProjectOverviewEntry* entry = dynamic_cast<ProjectOverviewEntry*>(topItem);
		if (entry) {
			auto it = m_categoryItems.find(entry->getProjectInformation().getCategory());
			if (it != m_categoryItems.end()) {
				m_tree->takeTopLevelItem(i);
				it->second->addChild(entry);
			}
		}
	}

	// Update category visibility
	int visibleCategories = 0;
	for (auto& it : m_categoryItems) {
		TreeWidgetItem* categoryItem = it.second;
		updateCategoryItem(categoryItem);
		if (!categoryItem->isHidden()) {
			visibleCategories++;
		}
	}

	// If only one category is visible, reparent items to root
	if (visibleCategories == 1) {
		for (auto& it : m_categoryItems) {
			TreeWidgetItem* categoryItem = it.second;
			if (!categoryItem->isHidden()) {
				for (int i = categoryItem->childCount() - 1; i >= 0; i--) {
					QTreeWidgetItem* child = categoryItem->child(i);
					categoryItem->removeChild(child);
					m_tree->addTopLevelItem(child);
				}
				categoryItem->setHidden(true);
			}
		}
	}
}

void ot::ProjectOverviewWidget::updateCategoryItem(TreeWidgetItem* _categoryItem) {
	int visibleItems = 0;
	for (int i = 0; i < _categoryItem->childCount(); i++) {
		if (!_categoryItem->child(i)->isHidden()) {
			visibleItems++;
		}
	}

	_categoryItem->setHidden(visibleItems == 0);
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
}
