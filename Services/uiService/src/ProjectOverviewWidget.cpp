//! @file ProjectOverviewWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "AppBase.h"
#include "UserManagement.h"
#include "ProjectManagement.h"
#include "ProjectOverviewWidget.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/TableItem.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/SignalBlockWrapper.h"

// TabToolbar
#include <TabToolbar/Page.h>
#include <TabToolbar/Group.h>

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>
#include <QtOpenGLWidgets/qopenglwidget.h>

enum TableColumn {
	ColumnCheck,
	ColumnType,
	ColumnName,
	ColumnOwner,
	ColumnGroups,
	ColumnLastAccess,
	ColumnCount
};

ProjectOverviewEntry::ProjectOverviewEntry(const ProjectInformation& _projectInfo, const QIcon& _projectTypeIcon, bool _ownerIsCreator, QTableWidget* _table)
	: m_ownerIsCreator(_ownerIsCreator), m_table(_table)
{
	int row = _table->rowCount();
	_table->insertRow(row);

	m_checkBox = new ot::CheckBox;
	m_checkBox->setFocusPolicy(Qt::NoFocus);
	
	m_typeItem = new ot::TableItem;
	m_typeItem->setFlags(m_typeItem->flags() & ~Qt::ItemIsEditable);
	m_typeItem->setIcon(_projectTypeIcon);
	m_typeItem->setToolTip(QString::fromStdString(_projectInfo.getProjectType()));
	m_typeItem->setSortHint(QString::fromStdString(_projectInfo.getProjectType()));

	m_nameItem = new ot::TableItem;
	m_nameItem->setFlags(m_typeItem->flags());
	m_nameItem->setText(QString::fromStdString(_projectInfo.getProjectName()));

	m_ownerItem = new ot::TableItem;
	m_ownerItem->setFlags(m_typeItem->flags());
	m_ownerItem->setText(QString::fromStdString(_projectInfo.getUserName()));

	m_groupsItem = new ot::TableItem;;
	m_groupsItem->setFlags(m_typeItem->flags());
	if (!_projectInfo.getGroups().empty()) {
		m_groupsItem->setIcon(ot::IconManager::getIcon("Default/Groups.png"));
		QString tip;
		if (_projectInfo.getGroups().size() == 1) {
			tip = "Shared with group:";
		}
		else {
			tip = "Shared with groups:";
		}

		std::string newGroupsSortHint;
		for (const std::string& group : _projectInfo.getGroups()) {
			tip.append("\n  - " + QString::fromStdString(group));
			newGroupsSortHint.push_back('\n');
			newGroupsSortHint.append(group);
		}
		m_groupsItem->setToolTip(tip);			
		m_groupsItem->setSortHint(QString::fromStdString(newGroupsSortHint));
	}

	m_lastAccessTimeItem = new ot::TableItem;
	m_lastAccessTimeItem->setFlags(m_typeItem->flags());
	m_lastAccessTimeItem->setText(_projectInfo.getLastAccessTime().toString());
	m_lastAccessTimeItem->setSortHint(_projectInfo.getLastAccessTime().toString("yyyy.MM.dd hh:mm:ss"));

	_table->setCellWidget(row, TableColumn::ColumnCheck, m_checkBox);
	_table->setItem(row, TableColumn::ColumnType, m_typeItem);
	_table->setItem(row, TableColumn::ColumnName, m_nameItem);
	_table->setItem(row, TableColumn::ColumnOwner, m_ownerItem);
	_table->setItem(row, TableColumn::ColumnGroups, m_groupsItem);
	_table->setItem(row, TableColumn::ColumnLastAccess, m_lastAccessTimeItem);

	this->connect(m_checkBox, &ot::CheckBox::stateChanged, this, &ProjectOverviewEntry::slotCheckedChanged);
}

void ProjectOverviewEntry::setIsChecked(bool _checked) {
	m_checkBox->setChecked(_checked);
}

bool ProjectOverviewEntry::getIsChecked(void) const {
	return m_checkBox->isChecked();
}

QString ProjectOverviewEntry::getProjectName(void) const {
	return m_nameItem->text();
}

void ProjectOverviewEntry::slotCheckedChanged(void) {
	bool isBlock = m_table->signalsBlocked();
	m_table->blockSignals(true);
	m_typeItem->setSelected(m_checkBox->isChecked());
	m_nameItem->setSelected(m_checkBox->isChecked());
	m_ownerItem->setSelected(m_checkBox->isChecked());
	m_groupsItem->setSelected(m_checkBox->isChecked());
	m_lastAccessTimeItem->setSelected(m_checkBox->isChecked());
	m_table->blockSignals(isBlock);
	Q_EMIT checkedChanged();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ProjectOverviewWidget::ProjectOverviewWidget(tt::Page* _ttbPage)
	: m_mode(ViewMode::ViewAll)
{
	// Create layouts
	m_widget = new QWidget;
	QVBoxLayout* centralLayout = new QVBoxLayout(m_widget);

	// Crete controls
	ot::Label* welcomeLabel = new ot::Label("Projects");
	m_filter = new ot::LineEdit;
	m_table = new ot::Table(0, TableColumn::ColumnCount);
	m_countLabel = new ot::Label;
	QOpenGLWidget* glWidget = new QOpenGLWidget;

	// Create Menu
	tt::Group* projectGroup = _ttbPage->AddGroup("Project");
	projectGroup->setObjectName("OT_TTBGroup");
	tt::Group* editGroup = _ttbPage->AddGroup("Edit Project");
	editGroup->setObjectName("OT_TTBGroup");

	m_createButton = this->iniToolButton("Create", "ToolBar/CreateProject.png", projectGroup, "Create new project");
	m_refreshButton = this->iniToolButton("Refresh", "ToolBar/RefreshProjects.png", projectGroup, "Refresh project list");
	m_toggleViewModeButton = this->iniToolButton("View Recent", "ToolBar/RefreshProjects.png", projectGroup, "Show recent projects");
	
	m_openButton = this->iniToolButton("Open", "ToolBar/OpenProject.png", editGroup, "Open selected project");
	m_copyButton = this->iniToolButton("Copy", "ToolBar/CopyProject.png", editGroup, "Copy selected project");
	m_renameButton = this->iniToolButton("Rename", "ToolBar/RenameProject.png", editGroup, "Rename selected project");
	m_deleteButton = this->iniToolButton("Delete", "ToolBar/DeleteProject.png", editGroup, "Delete selected project(s)");
	m_exportButton = this->iniToolButton("Export", "ToolBar/ExportProject.png", editGroup, "Export selected project(s)");
	m_accessButton = this->iniToolButton("Access", "ToolBar/ManageProjectAccess.png", editGroup, "Manage Access for selected projects");
	m_ownerButton = this->iniToolButton("Owner", "ToolBar/ChangeProjectOwner.png", editGroup, "Manage Owner of selected projects");

	// Setup controls
	QFont welcomeFont = welcomeLabel->font();
	welcomeFont.setPixelSize(28);
	welcomeLabel->setFont(welcomeFont);

	m_filter->setPlaceholderText("Find...");
	m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode(TableColumn::ColumnName, QHeaderView::Stretch);
	m_table->horizontalHeader()->installEventFilter(this);
	m_table->setHorizontalHeaderLabels({ "", "Type", "Name", "Owner", "Groups", "Last Modified" });
	m_table->horizontalHeader()->setSelectionMode(QAbstractItemView::NoSelection);
	m_table->setSortingEnabled(true);
	m_table->horizontalHeader()->setSortIndicatorShown(false);
	m_table->verticalHeader()->setHidden(true);

	glWidget->setMaximumSize(1, 1);

	this->updateCountLabel(false);
	this->updateToggleViewModeButton();
	this->updateToolButtonsEnabledState();

	// Setup layouts
	centralLayout->addWidget(welcomeLabel);
	centralLayout->addWidget(m_filter);
	centralLayout->addWidget(m_table);
	centralLayout->addWidget(m_countLabel);
	centralLayout->addWidget(glWidget);

	// Connect signals
	this->connect(m_filter, &ot::LineEdit::textChanged, this, &ProjectOverviewWidget::slotFilterChanged);
	this->connect(m_table, &ot::Table::cellDoubleClicked, this, &ProjectOverviewWidget::slotProjectDoubleClicked);
	this->connect(m_table, &ot::Table::itemSelectionChanged, this, &ProjectOverviewWidget::slotUpdateItemSelection);
	this->connect(m_table->horizontalHeader(), &QHeaderView::sectionClicked, this, &ProjectOverviewWidget::slotTableHeaderItemClicked);
	this->connect(m_table->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &ProjectOverviewWidget::slotTableHeaderSortingChanged);
	this->connect(m_createButton, &ot::ToolButton::clicked, this, &ProjectOverviewWidget::slotCreateProject);
	this->connect(m_refreshButton, &ot::ToolButton::clicked, this, &ProjectOverviewWidget::slotRefreshProjectList);
	this->connect(m_toggleViewModeButton, &ot::ToolButton::clicked, this, &ProjectOverviewWidget::slotToggleViewMode);
	this->connect(m_openButton, &ot::ToolButton::clicked, this, &ProjectOverviewWidget::slotOpenProject);
	this->connect(m_copyButton, &ot::ToolButton::clicked, this, &ProjectOverviewWidget::slotCopyProject);
	this->connect(m_renameButton, &ot::ToolButton::clicked, this, &ProjectOverviewWidget::slotRenameProject);
	this->connect(m_deleteButton, &ot::ToolButton::clicked, this, &ProjectOverviewWidget::slotDeleteProject);
	this->connect(m_exportButton, &ot::ToolButton::clicked, this, &ProjectOverviewWidget::slotExportProject);
	this->connect(m_accessButton, &ot::ToolButton::clicked, this, &ProjectOverviewWidget::slotAccessProject);
	this->connect(m_ownerButton, &ot::ToolButton::clicked, this, &ProjectOverviewWidget::slotOwnerProject);
}

ProjectOverviewWidget::~ProjectOverviewWidget() {

}

void ProjectOverviewWidget::setWidgetLocked(bool _isLocked) {
	m_widget->setEnabled(!_isLocked);
	this->updateToolButtonsEnabledState(_isLocked);
	m_createButton->setEnabled(!_isLocked);
	m_refreshButton->setEnabled(!_isLocked);
	m_toggleViewModeButton->setEnabled(!_isLocked);
}

QString ProjectOverviewWidget::getCurrentProjectFilter(void) const {
	return m_filter->text();
}

std::list<QString> ProjectOverviewWidget::getSelectedProjects(void) const {
	std::list<QString> result;
	for (ProjectOverviewEntry* entry : m_entries) {
		if (entry->getIsChecked()) {
			result.push_back(entry->getProjectName());
		}
	}
	return result;
}

void ProjectOverviewWidget::refreshProjectList(void) {
	this->slotRefreshProjectList();
}

void ProjectOverviewWidget::refreshRecentProjects(void) {
	this->slotRefreshRecentProjects();
}

void ProjectOverviewWidget::slotUpdateItemSelection(void) {
	QList<QTableWidgetItem*> selection = m_table->selectedItems();
	bool isBlock = m_table->signalsBlocked();
	m_table->blockSignals(true);
	
	// Table to check box
	for (int r = 0; r < m_table->rowCount(); r++) {
		if (r >= m_entries.size()) {
			OT_LOG_EA("Data mismatch");
			break;
		}

		ProjectOverviewEntry* entry = this->findEntry(m_table->item(r, TableColumn::ColumnName)->text());
		if (entry) {
			entry->setIsChecked(m_table->item(r, TableColumn::ColumnType)->isSelected() || m_table->item(r, TableColumn::ColumnName)->isSelected() || m_table->item(r, TableColumn::ColumnOwner)->isSelected() || m_table->item(r, TableColumn::ColumnGroups)->isSelected() || m_table->item(r, TableColumn::ColumnLastAccess)->isSelected());
		}
		else {
			OT_LOG_E("Entry not found for project \"" + m_table->item(r, TableColumn::ColumnName)->text().toStdString() + "\"");
		}
	}

	m_table->blockSignals(isBlock);
}

void ProjectOverviewWidget::slotCreateProject(void) {
	Q_EMIT createProjectRequest();
}

void ProjectOverviewWidget::slotProjectDoubleClicked(int _row, int _column) {
	if (_row < 0 || _row >= m_entries.size()) {
		OT_LOG_EA("Index out of range");
		return;
	}
	for (ProjectOverviewEntry* entry : m_entries) {
		entry->setIsChecked(false);
	}

	ProjectOverviewEntry* entry = this->findEntry(m_table->item(_row, TableColumn::ColumnName)->text());
	if (entry) {
		entry->setIsChecked(true);
		this->slotOpenProject();
	}
	else {
		OT_LOG_E("Entry not found for project \"" + m_table->item(_row, TableColumn::ColumnName)->text().toStdString() + "\"");
	}
}

void ProjectOverviewWidget::slotTableHeaderItemClicked(int _column) {
	if (_column == TableColumn::ColumnCheck) {
		bool allChecked = true;
		for (ProjectOverviewEntry* entry : m_entries) {
			if (!entry->getIsChecked()) {
				allChecked = false;
				ot::SignalBlockWrapper sigBlock(m_table);
				entry->setIsChecked(true);
			}
		}

		if (allChecked) {
			ot::SignalBlockWrapper sigBlock(m_table);
			for (ProjectOverviewEntry* entry : m_entries) {
				entry->setIsChecked(false);
			}
		}
	}
}

void ProjectOverviewWidget::slotTableHeaderSortingChanged(int _column, Qt::SortOrder _order) {
	m_table->horizontalHeader()->setSortIndicatorShown(_column != TableColumn::ColumnCheck);
}

void ProjectOverviewWidget::slotRefreshProjectList(void) {
	switch (m_mode) {
	case ProjectOverviewWidget::ViewMode::ViewAll:
		this->slotRefreshAllProjects();
		break;
	case ProjectOverviewWidget::ViewMode::ViewRecent:
		this->slotRefreshRecentProjects();
		break;
	default:
		OT_LOG_EA("Unknown view mode");
		break;
	}
}

void ProjectOverviewWidget::slotRefreshRecentProjects(void) {
	this->clear();

	m_mode = ViewMode::ViewRecent;

	AppBase* app = AppBase::instance();
	std::string currentUser = app->getCurrentLoginData().getUserName();

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
		ProjectInformation newInfo = projectManager.getProjectInformation(proj);

		if (newInfo.getProjectName().empty()) {
			OT_LOG_E("Project information for project \"" + proj + "\" not found");
		}
		else {
			this->addProject(newInfo, newInfo.getUserName() == currentUser);
		}
	}

	this->updateCountLabel(false);
	this->updateToggleViewModeButton();
	this->updateToolButtonsEnabledState();
}

void ProjectOverviewWidget::slotRefreshAllProjects(void) {
	this->clear();

	m_mode = ViewMode::ViewAll;

	AppBase* app = AppBase::instance();
	std::string currentUser = app->getCurrentLoginData().getUserName();

	std::list<ProjectInformation> projects;
	bool resultExceeded = false;
	ProjectManagement projectManager(app->getCurrentLoginData());
	projectManager.findProjectNames(m_filter->text().toStdString(), 100, projects, resultExceeded);

	for (const ProjectInformation& proj : projects) {
		std::string editorName("< Unknown >");
		//projectManager.getProjectAuthor(proj.getProjectName(), editorName);

		this->addProject(proj, proj.getUserName() == currentUser);
	}

	this->updateCountLabel(resultExceeded);
	this->updateToggleViewModeButton();
	this->updateToolButtonsEnabledState();
}

void ProjectOverviewWidget::slotToggleViewMode(void) {
	switch (m_mode) {
	case ProjectOverviewWidget::ViewMode::ViewAll:
		m_mode = ViewMode::ViewRecent;
		break;
	case ProjectOverviewWidget::ViewMode::ViewRecent:
		m_mode = ViewMode::ViewAll;
		break;
	default:
		OT_LOG_EA("Unknown view mode");
		break;
	}

	this->slotRefreshProjectList();
}

void ProjectOverviewWidget::slotOpenProject(void) {
	Q_EMIT openProjectRequest();
}

void ProjectOverviewWidget::slotCopyProject(void) {
	Q_EMIT copyProjectRequest();
}

void ProjectOverviewWidget::slotRenameProject(void) {
	Q_EMIT renameProjectRequest();
}

void ProjectOverviewWidget::slotDeleteProject(void) {
	Q_EMIT deleteProjectRequest();
}

void ProjectOverviewWidget::slotExportProject(void) {
	Q_EMIT exportProjectRequest();
}

void ProjectOverviewWidget::slotAccessProject(void) {
	Q_EMIT projectAccessRequest();
	this->refreshProjectList();
}

void ProjectOverviewWidget::slotOwnerProject(void) {
	Q_EMIT projectOwnerRequest();
	this->refreshProjectList();
}

void ProjectOverviewWidget::slotFilterChanged(void) {
	if (m_filter->text().isEmpty()) {
		for (int r = 0; r < m_table->rowCount(); r++) {
			m_table->setRowHidden(r, false);
		}
	}
	else {
		for (int r = 0; r < m_table->rowCount(); r++) {
			OTAssert(r < m_entries.size(), "Index mismatch");
			m_table->setRowHidden(r, !(m_table->item(r, TableColumn::ColumnName)->text().contains(m_filter->text(), Qt::CaseInsensitive)));
		}
	}
}

void ProjectOverviewWidget::slotProjectCheckedChanged(void) {
	this->updateToolButtonsEnabledState();
}

ot::ToolButton* ProjectOverviewWidget::iniToolButton(const QString& _text, const QString& _iconPath, tt::Group* _group, const QString& _toolTip) {
	ot::ToolButton* newButton = new ot::ToolButton(ot::IconManager::getIcon(_iconPath), _text);
	const int iconSize = QApplication::style()->pixelMetric(QStyle::PM_LargeIconSize);
	newButton->setAutoRaise(true);
	newButton->setIconSize(QSize(iconSize, iconSize));
	newButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	newButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	newButton->setToolTipDuration(3000);
	newButton->setToolTip(_toolTip);

	_group->AddWidget(newButton);

	return newButton;
}

void ProjectOverviewWidget::clear(void) {
	ot::SignalBlockWrapper sigBlock(m_table);
	int ix = m_table->horizontalHeader()->sortIndicatorSection();
	Qt::SortOrder order = m_table->horizontalHeader()->sortIndicatorOrder();

	bool wasSortingEnabled = m_table->isSortingEnabled();
	m_table->setSortingEnabled(false);

	for (ProjectOverviewEntry* entry : m_entries) {
		this->disconnect(entry, &ProjectOverviewEntry::checkedChanged, this, &ProjectOverviewWidget::slotProjectCheckedChanged);
		delete entry;
	}
	m_entries.clear();
	m_table->setRowCount(0);

	if (wasSortingEnabled) {
		m_table->setSortingEnabled(true);
		if (ix >= 0) {
			m_table->horizontalHeader()->setSortIndicator(ix, order);
			this->slotTableHeaderSortingChanged(ix, order);
		}
	}
}

void ProjectOverviewWidget::addProject(const ProjectInformation& _projectInfo, bool _ownerIsCreator) {
	ot::SignalBlockWrapper sigBlock(m_table);
	
	int ix = m_table->horizontalHeader()->sortIndicatorSection();
	Qt::SortOrder order = m_table->horizontalHeader()->sortIndicatorOrder();

	bool wasSortingEnabled = m_table->isSortingEnabled();
	m_table->setSortingEnabled(false);

	QIcon projectTypeIcon = AppBase::instance()->getDefaultProjectTypeIcon();
	auto iconIt = AppBase::instance()->getProjectTypeDefaultIconNameMap().find(_projectInfo.getProjectType());
	if (iconIt != AppBase::instance()->getProjectTypeDefaultIconNameMap().end()) {
		projectTypeIcon = ot::IconManager::getIcon(QString::fromStdString(iconIt->second));
	}

	ProjectOverviewEntry* newEntry = new ProjectOverviewEntry(_projectInfo, projectTypeIcon, _ownerIsCreator, m_table);
	m_entries.push_back(newEntry);
	this->connect(newEntry, &ProjectOverviewEntry::checkedChanged, this, &ProjectOverviewWidget::slotProjectCheckedChanged);

	if (wasSortingEnabled) {
		m_table->setSortingEnabled(true);
		if (ix >= 0) {
			m_table->horizontalHeader()->setSortIndicator(ix, order);
			this->slotTableHeaderSortingChanged(ix, order);
		}
	}
}

void ProjectOverviewWidget::updateCountLabel(bool _hasMore) {
	if (m_entries.empty()) {
		m_countLabel->setText("No projects found");
	}
	else if (m_entries.size() == 1) {
		if (_hasMore) {
			m_countLabel->setText("Showing the first project found");
		}
		else {
			m_countLabel->setText("1 project found");
		}
	}
	else {
		if (_hasMore) {
			m_countLabel->setText("Showing the first " + QString::number(m_entries.size()) + " projects");
		}
		else {
			m_countLabel->setText(QString::number(m_entries.size()) + " projects found");
		}
	}
}

void ProjectOverviewWidget::updateToggleViewModeButton(void) {
	switch (m_mode) {
	case ProjectOverviewWidget::ViewMode::ViewAll:
		m_toggleViewModeButton->setText("View Recent");
		m_toggleViewModeButton->setIcon(ot::IconManager::getIcon("ToolBar/ViewRecentProjects.png"));
		m_toggleViewModeButton->setToolTip("View recent projects (Currently showing all projects)");
		break;
	case ProjectOverviewWidget::ViewMode::ViewRecent:
		m_toggleViewModeButton->setText("View All");
		m_toggleViewModeButton->setIcon(ot::IconManager::getIcon("ToolBar/ViewAllProjects.png"));
		m_toggleViewModeButton->setToolTip("View all projects (Currently showing recent projects)");
		break;
	default:
		OT_LOG_EA("Unknown view mode");
		break;
	}
}

void ProjectOverviewWidget::updateToolButtonsEnabledState(bool _forceDisabled) {
	std::list<QString> lst = this->getSelectedProjects();
	bool hasDifferentOwner = this->hasDifferentSelectedOwner();

	if (lst.empty() || _forceDisabled) {
		m_openButton->setEnabled(false);
		m_copyButton->setEnabled(false);
		m_renameButton->setEnabled(false);
		m_deleteButton->setEnabled(false);
		m_exportButton->setEnabled(false);
		m_accessButton->setEnabled(false);
		m_ownerButton->setEnabled(false);
	}
	else if (lst.size() == 1) {
		m_openButton->setEnabled(true);
		m_copyButton->setEnabled(true);
		m_renameButton->setEnabled(!hasDifferentOwner);
		m_deleteButton->setEnabled(!hasDifferentOwner);
		m_exportButton->setEnabled(true);
		m_accessButton->setEnabled(!hasDifferentOwner);
		m_ownerButton->setEnabled(!hasDifferentOwner);
	}
	else {
		m_openButton->setEnabled(false);
		m_copyButton->setEnabled(false);
		m_renameButton->setEnabled(false);
		m_deleteButton->setEnabled(!hasDifferentOwner);
		m_exportButton->setEnabled(false);
		m_accessButton->setEnabled(false);
		m_ownerButton->setEnabled(false);
	}
}

bool ProjectOverviewWidget::hasDifferentSelectedOwner(void) {
	for (ProjectOverviewEntry* entry : m_entries) {
		if (entry->getIsChecked()) {
			if (!entry->getOwnerIsCreator()) return true;
		}
	}
	return false;
}

ProjectOverviewEntry* ProjectOverviewWidget::findEntry(const QString& _projectName) {
	for (ProjectOverviewEntry* entry : m_entries) {
		if (entry->getProjectName() == _projectName) {
			return entry;
		}
	}
	return nullptr;
}
