//! @file ProjectOverviewWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "AppBase.h"
#include "WelcomeWidget.h"
#include "UserManagement.h"
#include "ProjectManagement.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/TableItem.h"
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
	ColumnType,
	ColumnName,
	ColumnOwner,
	ColumnGroups,
	ColumnLastAccess,
	ColumnCount
};

ProjectOverviewEntryOld::ProjectOverviewEntryOld(const ot::ProjectInformation& _projectInfo, const QIcon& _projectTypeIcon, bool _ownerIsCreator, QTableWidget* _table)
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

	QDateTime lastAccess = QDateTime::fromMSecsSinceEpoch(_projectInfo.getLastAccessTime());

	m_lastAccessTimeItem = new ot::TableItem;
	m_lastAccessTimeItem->setFlags(m_typeItem->flags());
	m_lastAccessTimeItem->setText(lastAccess.toString());
	m_lastAccessTimeItem->setSortHint(lastAccess.toString("yyyy.MM.dd hh:mm:ss"));

	_table->setCellWidget(row, TableColumn::ColumnCheck, m_checkBox);
	_table->setItem(row, TableColumn::ColumnType, m_typeItem);
	_table->setItem(row, TableColumn::ColumnName, m_nameItem);
	_table->setItem(row, TableColumn::ColumnOwner, m_ownerItem);
	_table->setItem(row, TableColumn::ColumnGroups, m_groupsItem);
	_table->setItem(row, TableColumn::ColumnLastAccess, m_lastAccessTimeItem);

	this->connect(m_checkBox, &ot::CheckBox::stateChanged, this, &ProjectOverviewEntryOld::slotCheckedChanged);
}

void ProjectOverviewEntryOld::setIsChecked(bool _checked) {
	m_checkBox->setChecked(_checked);
}

bool ProjectOverviewEntryOld::getIsChecked() const {
	return m_checkBox->isChecked();
}

QString ProjectOverviewEntryOld::getProjectName() const {
	return m_nameItem->text();
}

void ProjectOverviewEntryOld::slotCheckedChanged() {
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

WelcomeWidget::WelcomeWidget(tt::Page* _ttbPage)
	: m_mode(ViewMode::ViewAll)
{
	// Create layouts
	m_widget = new QWidget;
	QVBoxLayout* centralLayout = new QVBoxLayout(m_widget);

	// Crete controls
	m_titleLabel = new ot::Label("Projects");
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
	m_deleteButton->setToolTip("Delete (Del)");
	m_exportButton = this->iniToolButton("Export", "ToolBar/ExportProject.png", editGroup, "Export selected project(s)");
	m_accessButton = this->iniToolButton("Access", "ToolBar/ManageProjectAccess.png", editGroup, "Manage Access for selected projects");
	m_ownerButton = this->iniToolButton("Owner", "ToolBar/ChangeProjectOwner.png", editGroup, "Manage Owner of selected projects");

	// Setup controls
	QFont welcomeFont = m_titleLabel->font();
	welcomeFont.setPixelSize(28);
	m_titleLabel->setFont(welcomeFont);

	m_filter->setPlaceholderText("Find...");
	m_filter->installEventFilter(this);

	m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode(TableColumn::ColumnName, QHeaderView::Stretch);
	m_table->horizontalHeader()->installEventFilter(this);
	m_table->setHorizontalHeaderLabels({ "", "Type", "Name", "Owner", "Groups", "Last Modified" });
	m_table->horizontalHeader()->setSelectionMode(QAbstractItemView::NoSelection);
	m_table->setSortingEnabled(true);
	m_table->horizontalHeader()->setSortIndicatorShown(false);
	m_table->verticalHeader()->setHidden(true);
	m_table->installEventFilter(this);

	glWidget->setMaximumSize(1, 1);

	this->updateCountLabel(false);
	this->updateToggleViewModeButton();
	this->updateToolButtonsEnabledState();

	// Setup layouts
	centralLayout->addWidget(m_titleLabel);
	centralLayout->addWidget(m_filter);
	centralLayout->addWidget(m_table);
	centralLayout->addWidget(m_countLabel);
	centralLayout->addWidget(glWidget);

	// Connect signals
	this->connect(m_filter, &ot::LineEdit::textChanged, this, &WelcomeWidget::slotFilterChanged);
	this->connect(m_table, &ot::Table::cellDoubleClicked, this, &WelcomeWidget::slotProjectDoubleClicked);
	this->connect(m_table, &ot::Table::itemSelectionChanged, this, &WelcomeWidget::slotUpdateItemSelection);
	this->connect(m_table->horizontalHeader(), &QHeaderView::sectionClicked, this, &WelcomeWidget::slotTableHeaderItemClicked);
	this->connect(m_table->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &WelcomeWidget::slotTableHeaderSortingChanged);
	this->connect(m_createButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotCreateProject);
	this->connect(m_refreshButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotRefreshProjectList);
	this->connect(m_toggleViewModeButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotToggleViewMode);
	this->connect(m_openButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotOpenProject);
	this->connect(m_copyButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotCopyProject);
	this->connect(m_renameButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotRenameProject);
	this->connect(m_deleteButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotDeleteProject);
	this->connect(m_exportButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotExportProject);
	this->connect(m_accessButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotAccessProject);
	this->connect(m_ownerButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotOwnerProject);
}

WelcomeWidget::~WelcomeWidget() {

}

void WelcomeWidget::setWidgetLocked(bool _isLocked) {
	m_widget->setEnabled(!_isLocked);
	this->updateToolButtonsEnabledState(_isLocked);
	m_createButton->setEnabled(!_isLocked);
	m_refreshButton->setEnabled(!_isLocked);
	m_toggleViewModeButton->setEnabled(!_isLocked);
}

bool WelcomeWidget::eventFilter(QObject* _watched, QEvent* _event) {
	if (_watched == m_table && _event->type() == QEvent::KeyPress) {
		QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(_event);
		if (!keyEvent) {
			OT_LOG_E("Key event is null");
			return false;
		}
		if (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
			this->slotDeleteProject();
			return true;
		}
	}
	else if (_watched == m_filter && _event->type() == QEvent::KeyPress) {
		QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(_event);
		if (!keyEvent) {
			OT_LOG_E("Key event is null");
			return false;
		}
		if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
			this->slotCreateProject();
			return true;
		}
	}

	return false;
}

QString WelcomeWidget::getCurrentProjectFilter() const {
	return m_filter->text();
}

std::list<QString> WelcomeWidget::getSelectedProjects() const {
	std::list<QString> result;
	for (ProjectOverviewEntryOld* entry : m_entries) {
		if (entry->getIsChecked()) {
			result.push_back(entry->getProjectName());
		}
	}
	return result;
}

void WelcomeWidget::refreshProjectList() {
	this->slotRefreshProjectList();
}

void WelcomeWidget::refreshRecentProjects() {
	this->slotRefreshRecentProjects();
}

void WelcomeWidget::slotUpdateItemSelection() {
	QList<QTableWidgetItem*> selection = m_table->selectedItems();
	bool isBlock = m_table->signalsBlocked();
	m_table->blockSignals(true);
	
	// Table to check box
	for (int r = 0; r < m_table->rowCount(); r++) {
		if (r >= m_entries.size()) {
			OT_LOG_EA("Data mismatch");
			break;
		}

		ProjectOverviewEntryOld* entry = this->findEntry(m_table->item(r, TableColumn::ColumnName)->text());
		if (entry) {
			entry->setIsChecked(m_table->item(r, TableColumn::ColumnType)->isSelected() || m_table->item(r, TableColumn::ColumnName)->isSelected() || m_table->item(r, TableColumn::ColumnOwner)->isSelected() || m_table->item(r, TableColumn::ColumnGroups)->isSelected() || m_table->item(r, TableColumn::ColumnLastAccess)->isSelected());
		}
		else {
			OT_LOG_E("Entry not found for project \"" + m_table->item(r, TableColumn::ColumnName)->text().toStdString() + "\"");
		}
	}

	m_table->blockSignals(isBlock);
}

void WelcomeWidget::slotCreateProject() {
	Q_EMIT createProjectRequest();
}

void WelcomeWidget::slotProjectDoubleClicked(int _row, int _column) {
	if (_row < 0 || _row >= m_entries.size()) {
		OT_LOG_EA("Index out of range");
		return;
	}
	for (ProjectOverviewEntryOld* entry : m_entries) {
		entry->setIsChecked(false);
	}

	ProjectOverviewEntryOld* entry = this->findEntry(m_table->item(_row, TableColumn::ColumnName)->text());
	if (entry) {
		entry->setIsChecked(true);
		this->slotOpenProject();
	}
	else {
		OT_LOG_E("Entry not found for project \"" + m_table->item(_row, TableColumn::ColumnName)->text().toStdString() + "\"");
	}
}

void WelcomeWidget::slotTableHeaderItemClicked(int _column) {
	if (_column == TableColumn::ColumnCheck) {
		bool allChecked = true;
		for (ProjectOverviewEntryOld* entry : m_entries) {
			if (!entry->getIsChecked()) {
				allChecked = false;
				QSignalBlocker sigBlock(m_table);
				entry->setIsChecked(true);
			}
		}

		if (allChecked) {
			QSignalBlocker sigBlock(m_table);
			for (ProjectOverviewEntryOld* entry : m_entries) {
				entry->setIsChecked(false);
			}
		}
	}
}

void WelcomeWidget::slotTableHeaderSortingChanged(int _column, Qt::SortOrder _order) {
	m_table->horizontalHeader()->setSortIndicatorShown(_column != TableColumn::ColumnCheck);
}

void WelcomeWidget::slotRefreshProjectList() {
	switch (m_mode) {
	case WelcomeWidget::ViewMode::ViewAll:
		this->slotRefreshAllProjects();
		break;
	case WelcomeWidget::ViewMode::ViewRecent:
		this->slotRefreshRecentProjects();
		break;
	default:
		OT_LOG_EA("Unknown view mode");
		break;
	}
}

void WelcomeWidget::slotRefreshRecentProjects() {
	this->clear();

	m_mode = ViewMode::ViewRecent;
	m_titleLabel->setText("Recent Projects");

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
		ot::ProjectInformation newInfo = projectManager.getProjectInformation(proj);

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

void WelcomeWidget::slotRefreshAllProjects() {
	this->clear();

	m_mode = ViewMode::ViewAll;
	m_titleLabel->setText("Projects");

	AppBase* app = AppBase::instance();
	std::string currentUser = app->getCurrentLoginData().getUserName();

	std::list<ot::ProjectInformation> projects;
	bool resultExceeded = false;
	ProjectManagement projectManager(app->getCurrentLoginData());
	projectManager.findProjects(m_filter->text().toStdString(), 100, projects, resultExceeded);

	for (const ot::ProjectInformation& proj : projects) {
		std::string editorName("< Unknown >");
		//projectManager.getProjectAuthor(proj.getProjectName(), editorName);

		this->addProject(proj, proj.getUserName() == currentUser);
	}

	this->updateCountLabel(resultExceeded);
	this->updateToggleViewModeButton();
	this->updateToolButtonsEnabledState();
}

void WelcomeWidget::slotToggleViewMode() {
	switch (m_mode) {
	case WelcomeWidget::ViewMode::ViewAll:
		m_mode = ViewMode::ViewRecent;
		break;
	case WelcomeWidget::ViewMode::ViewRecent:
		m_mode = ViewMode::ViewAll;
		break;
	default:
		OT_LOG_EA("Unknown view mode");
		break;
	}

	this->slotRefreshProjectList();
}

void WelcomeWidget::slotOpenProject() {
	Q_EMIT openProjectRequest();
}

void WelcomeWidget::slotCopyProject() {
	Q_EMIT copyProjectRequest();
}

void WelcomeWidget::slotRenameProject() {
	Q_EMIT renameProjectRequest();
}

void WelcomeWidget::slotDeleteProject() {
	Q_EMIT deleteProjectRequest();
}

void WelcomeWidget::slotExportProject() {
	Q_EMIT exportProjectRequest();
}

void WelcomeWidget::slotAccessProject() {
	Q_EMIT projectAccessRequest();
	this->refreshProjectList();
}

void WelcomeWidget::slotOwnerProject() {
	Q_EMIT projectOwnerRequest();
	this->refreshProjectList();
}

void WelcomeWidget::slotFilterChanged() {
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

void WelcomeWidget::slotProjectCheckedChanged() {
	this->updateToolButtonsEnabledState();
}

ot::ToolButton* WelcomeWidget::iniToolButton(const QString& _text, const QString& _iconPath, tt::Group* _group, const QString& _toolTip) {
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

void WelcomeWidget::clear() {
	QSignalBlocker sigBlock(m_table);
	int ix = m_table->horizontalHeader()->sortIndicatorSection();
	Qt::SortOrder order = m_table->horizontalHeader()->sortIndicatorOrder();

	bool wasSortingEnabled = m_table->isSortingEnabled();
	m_table->setSortingEnabled(false);

	for (ProjectOverviewEntryOld* entry : m_entries) {
		this->disconnect(entry, &ProjectOverviewEntryOld::checkedChanged, this, &WelcomeWidget::slotProjectCheckedChanged);
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

void WelcomeWidget::addProject(const ot::ProjectInformation& _projectInfo, bool _ownerIsCreator) {
	QSignalBlocker sigBlock(m_table);
	
	int ix = m_table->horizontalHeader()->sortIndicatorSection();
	Qt::SortOrder order = m_table->horizontalHeader()->sortIndicatorOrder();

	bool wasSortingEnabled = m_table->isSortingEnabled();
	m_table->setSortingEnabled(false);

	QIcon projectTypeIcon;
	const QString iconPath = "ProjectTemplates/" + QString::fromStdString(_projectInfo.getProjectType()) + ".png";
	if (ot::IconManager::fileExists(iconPath)) {
		projectTypeIcon = ot::IconManager::getIcon(iconPath);
	}
	else {
		projectTypeIcon = ot::IconManager::getDefaultProjectIcon();
	}

	ProjectOverviewEntryOld* newEntry = new ProjectOverviewEntryOld(_projectInfo, projectTypeIcon, _ownerIsCreator, m_table);
	m_entries.push_back(newEntry);
	this->connect(newEntry, &ProjectOverviewEntryOld::checkedChanged, this, &WelcomeWidget::slotProjectCheckedChanged);

	if (wasSortingEnabled) {
		m_table->setSortingEnabled(true);
		if (ix >= 0) {
			m_table->horizontalHeader()->setSortIndicator(ix, order);
			this->slotTableHeaderSortingChanged(ix, order);
		}
	}
}

void WelcomeWidget::updateCountLabel(bool _hasMore) {
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

void WelcomeWidget::updateToggleViewModeButton() {
	switch (m_mode) {
	case WelcomeWidget::ViewMode::ViewAll:
		m_toggleViewModeButton->setText("View Recent");
		m_toggleViewModeButton->setIcon(ot::IconManager::getIcon("ToolBar/ViewRecentProjects.png"));
		m_toggleViewModeButton->setToolTip("View recent projects (Currently showing all projects)");
		break;
	case WelcomeWidget::ViewMode::ViewRecent:
		m_toggleViewModeButton->setText("View All");
		m_toggleViewModeButton->setIcon(ot::IconManager::getIcon("ToolBar/ViewAllProjects.png"));
		m_toggleViewModeButton->setToolTip("View all projects (Currently showing recent projects)");
		break;
	default:
		OT_LOG_EA("Unknown view mode");
		break;
	}
}

void WelcomeWidget::updateToolButtonsEnabledState(bool _forceDisabled) {
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

bool WelcomeWidget::hasDifferentSelectedOwner() {
	for (ProjectOverviewEntryOld* entry : m_entries) {
		if (entry->getIsChecked()) {
			if (!entry->getOwnerIsCreator()) return true;
		}
	}
	return false;
}

ProjectOverviewEntryOld* WelcomeWidget::findEntry(const QString& _projectName) {
	for (ProjectOverviewEntryOld* entry : m_entries) {
		if (entry->getProjectName() == _projectName) {
			return entry;
		}
	}
	return nullptr;
}
