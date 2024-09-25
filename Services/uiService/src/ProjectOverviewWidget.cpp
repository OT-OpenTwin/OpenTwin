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
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/IconManager.h"

// TabToolbar
#include <TabToolbar/Page.h>
#include <TabToolbar/Group.h>

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>
#include <QtOpenGLWidgets/qopenglwidget.h>

enum TableColumn {
	ColumnCheck,
	ColumnName,
	ColumnOwner,
	ColumnCount
};

ProjectOverviewEntry::ProjectOverviewEntry(const QString& _projectName, const QString& _owner, bool _ownerIsCreator, QTableWidget* _table) 
	: m_ownerIsCreator(_ownerIsCreator)
{
	m_row = _table->rowCount();
	_table->insertRow(m_row);

	m_checkBox = new ot::CheckBox;
	
	m_nameItem = new QTableWidgetItem;
	m_nameItem->setFlags(m_nameItem->flags() & ~Qt::ItemIsEditable);
	m_nameItem->setText(_projectName);

	m_ownerItem = new QTableWidgetItem;
	m_ownerItem->setFlags(m_nameItem->flags());
	m_ownerItem->setText(_owner);

	_table->setCellWidget(m_row, TableColumn::ColumnCheck, m_checkBox);
	_table->setItem(m_row, TableColumn::ColumnName, m_nameItem);
	_table->setItem(m_row, TableColumn::ColumnOwner, m_ownerItem);

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
	m_nameItem->setSelected(m_checkBox->isChecked());
	m_ownerItem->setSelected(m_checkBox->isChecked());
	Q_EMIT checkedChanged(m_row);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ProjectOverviewWidget::ProjectOverviewWidget(tt::Page* _ttbPage)
	: m_lockCount(0), m_mode(ViewMode::ViewAll)
{
	// Create layouts
	m_widget = new QWidget;
	QVBoxLayout* centralLayout = new QVBoxLayout(m_widget);

	// Crete controls
	ot::Label* welcomeLabel = new ot::Label("Welcome");
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
	welcomeFont.setPixelSize(48);
	welcomeLabel->setFont(welcomeFont);

	m_filter->setPlaceholderText("Find...");
	m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode(TableColumn::ColumnName, QHeaderView::Stretch);
	m_table->setHorizontalHeaderLabels({ "", "Name", "Owner" });
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

void ProjectOverviewWidget::lock(bool _flag) {
	if (_flag) {
		if (m_lockCount == 0) {
			m_widget->setEnabled(false);
			this->updateToolButtonsEnabledState(true);
			m_createButton->setEnabled(false);
			m_refreshButton->setEnabled(false);
			m_toggleViewModeButton->setEnabled(false);
		}
		m_lockCount++;
	}
	else if (m_lockCount == 0) {
		OTAssert(0, "No lock set, check locks");
		return;
	}
	else {
		m_lockCount--;
		if (m_lockCount == 0) {
			m_widget->setEnabled(true);
			this->updateToolButtonsEnabledState();
			m_createButton->setEnabled(true);
			m_refreshButton->setEnabled(true);
			m_toggleViewModeButton->setEnabled(true);
		}
	}
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
	m_entries[_row]->setIsChecked(true);
	this->slotOpenProject();
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
	if (!projectManager.readProjectAuthor(recent)) {
		OT_LOG_E("Read project author failed");
		return;
	}
	
	for (const std::string& proj : recent) {
		std::string editorName("< Unknown >");
		projectManager.getProjectAuthor(proj, editorName);

		this->addProject(QString::fromStdString(proj), QString::fromStdString(editorName), editorName == currentUser);
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

	std::list<std::string> projects;
	bool resultExceeded = false;
	ProjectManagement projectManager(app->getCurrentLoginData());
	projectManager.findProjectNames(m_filter->text().toStdString(), 100, projects, resultExceeded);

	for (const std::string& proj : projects) {
		std::string editorName("< Unknown >");
		projectManager.getProjectAuthor(proj, editorName);

		this->addProject(QString::fromStdString(proj), QString::fromStdString(editorName), editorName == currentUser);
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
}

void ProjectOverviewWidget::slotOwnerProject(void) {
	Q_EMIT projectOwnerRequest();
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
			m_table->setRowHidden(r, !(m_entries[r]->getProjectName().contains(m_filter->text(), Qt::CaseInsensitive)));
		}
	}
}

void ProjectOverviewWidget::slotProjectCheckedChanged(int _row) {
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
	for (ProjectOverviewEntry* entry : m_entries) {
		this->disconnect(entry, &ProjectOverviewEntry::checkedChanged, this, &ProjectOverviewWidget::slotProjectCheckedChanged);
		delete entry;
	}
	m_entries.clear();
	m_table->setRowCount(0);
}

void ProjectOverviewWidget::addProject(const QString& _projectName, const QString& _owner, bool _ownerIsCreator) {
	ProjectOverviewEntry* newEntry = new ProjectOverviewEntry(_projectName, _owner, _ownerIsCreator, m_table);
	OTAssert(newEntry->getRow() == m_entries.size(), "Index mismatch");
	m_entries.push_back(newEntry);
	this->connect(newEntry, &ProjectOverviewEntry::checkedChanged, this, &ProjectOverviewWidget::slotProjectCheckedChanged);
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
