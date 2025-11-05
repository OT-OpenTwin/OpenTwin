// @otlicense
// File: WelcomeWidget.cpp
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

// Frontend header
#include "AppBase.h"
#include "WelcomeWidget.h"
#include "UserManagement.h"
#include "ProjectManagement.h"
#include "ProjectOverviewWidget.h"

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

WelcomeWidget::WelcomeWidget(tt::Page* _ttbPage, UserManagement& _userManager, QWidget* _parent) {
	// Create layouts
	m_widget = new QWidget(_parent);
	QVBoxLayout* centralLayout = new QVBoxLayout(m_widget);

	// Crete controls
	m_titleLabel = new ot::Label("Projects", m_widget);
	QFont welcomeFont = m_titleLabel->font();
	welcomeFont.setPixelSize(28);
	m_titleLabel->setFont(welcomeFont);
	centralLayout->addWidget(m_titleLabel);

	m_overview = new ot::ProjectOverviewWidget(m_widget);
	m_overview->setMultiSelectionEnabled(true);
	centralLayout->addWidget(m_overview->getQWidget());
	this->connect(m_overview, &ot::ProjectOverviewWidget::selectionChanged, this, &WelcomeWidget::slotSelectionChanged);
	this->connect(m_overview, &ot::ProjectOverviewWidget::projectOpenRequested, this, &WelcomeWidget::slotOpenProject);
	this->connect(m_overview, &ot::ProjectOverviewWidget::filterReturnPressed, this, &WelcomeWidget::slotCreateProject);

	QOpenGLWidget* glWidget = new QOpenGLWidget(m_widget);
	glWidget->setMaximumSize(1, 1);
	centralLayout->addWidget(glWidget);

	// Create Menu
	tt::Group* projectGroup = _ttbPage->AddGroup("Project");
	projectGroup->setObjectName("OT_TTBGroup");
	tt::Group* editGroup = _ttbPage->AddGroup("Edit Project");
	editGroup->setObjectName("OT_TTBGroup");

	m_createButton = this->iniToolButton("Create", "ToolBar/CreateProject.png", projectGroup, "Create new project");
	this->connect(m_createButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotCreateProject);

	m_refreshButton = this->iniToolButton("Refresh", "ToolBar/RefreshProjects.png", projectGroup, "Refresh project list");
	this->connect(m_refreshButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotRefreshProjectList);

	m_toggleViewModeButton = this->iniToolButton("List View", "ToolBar/ListView.png", projectGroup, "Switch to List View");
	this->connect(m_toggleViewModeButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotToggleViewMode);

	m_openButton = this->iniToolButton("Open", "ToolBar/OpenProject.png", editGroup, "Open selected project");
	this->connect(m_openButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotOpenProject);

	m_copyButton = this->iniToolButton("Copy", "ToolBar/CopyProject.png", editGroup, "Copy selected project");
	this->connect(m_copyButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotCopyProject);

	m_renameButton = this->iniToolButton("Rename", "ToolBar/RenameProject.png", editGroup, "Rename selected project");
	this->connect(m_renameButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotRenameProject);

	m_deleteButton = this->iniToolButton("Delete", "ToolBar/DeleteProject.png", editGroup, "Delete selected project(s)");
	m_deleteButton->setToolTip("Delete (Del)");
	this->connect(m_deleteButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotDeleteProject);

	m_exportButton = this->iniToolButton("Export", "ToolBar/ExportProject.png", editGroup, "Export selected project(s)");
	this->connect(m_exportButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotExportProject);

	m_accessButton = this->iniToolButton("Access", "ToolBar/ManageProjectAccess.png", editGroup, "Manage Access for selected projects");
	this->connect(m_accessButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotAccessProject);

	m_ownerButton = this->iniToolButton("Owner", "ToolBar/ChangeProjectOwner.png", editGroup, "Manage Owner of selected projects");
	this->connect(m_ownerButton, &ot::ToolButton::clicked, this, &WelcomeWidget::slotOwnerProject);

	this->updateToolButtonsEnabledState();

	// Initialize view mode
	std::string viewMode = _userManager.restoreSetting("WelcomeWidget_ViewMode");
	if (viewMode.empty()){
		setViewMode(ot::ProjectOverviewWidget::ViewMode::Tree);
	}
	else {
		setViewMode(ot::ProjectOverviewWidget::viewModeFromString(viewMode));
	}
}

WelcomeWidget::~WelcomeWidget() {

}

void WelcomeWidget::setWidgetLocked(bool _isLocked) {
	m_widget->setEnabled(!_isLocked);
	this->updateToolButtonsEnabledState(_isLocked);
	m_createButton->setEnabled(!_isLocked);
	m_refreshButton->setEnabled(!_isLocked);
}

QString WelcomeWidget::getCurrentQuickFilter() const {
	return m_overview->getCurrentQuickFilter();
}

std::list<ot::ProjectInformation> WelcomeWidget::getSelectedProjects() const {
	return m_overview->getSelectedProjects();
}

void WelcomeWidget::setViewMode(ot::ProjectOverviewWidget::ViewMode _mode) {
	m_overview->setViewMode(_mode);

	switch (_mode) {
	case ot::ProjectOverviewWidget::ViewMode::Tree:
		m_toggleViewModeButton->setIcon(ot::IconManager::getIcon("ToolBar/ListView.png"));
		m_toggleViewModeButton->setText("View as List");
		m_toggleViewModeButton->setToolTip("Switch to List View");
		break;
	case ot::ProjectOverviewWidget::ViewMode::List:
		m_toggleViewModeButton->setIcon(ot::IconManager::getIcon("ToolBar/TreeView.png"));
		m_toggleViewModeButton->setText("View as Tree");
		m_toggleViewModeButton->setToolTip("Switch to Tree View");
		break;
	default:
		OT_LOG_E("Invalid ViewMode (" + std::to_string(static_cast<int>(_mode)) + ")");
		break;
	}
}

void WelcomeWidget::storeViewModeSetting(UserManagement& _userManager) {
	_userManager.storeSetting("WelcomeWidget_ViewMode", ot::ProjectOverviewWidget::toString(m_overview->getViewMode()));
}

void WelcomeWidget::slotCreateProject() {
	Q_EMIT createProjectRequest();
}

void WelcomeWidget::slotRefreshProjectList() {
	m_overview->refreshProjects();
	m_overview->updateFilterOptions();

	this->updateToolButtonsEnabledState();
}

void WelcomeWidget::slotToggleViewMode() {
	if (m_overview->getViewMode() == ot::ProjectOverviewWidget::ViewMode::Tree) {
		setViewMode(ot::ProjectOverviewWidget::ViewMode::List);
	}
	else {
		setViewMode(ot::ProjectOverviewWidget::ViewMode::Tree);
	}
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
	this->slotRefreshProjectList();
}

void WelcomeWidget::slotOwnerProject() {
	Q_EMIT projectOwnerRequest();
	this->slotRefreshProjectList();
}

void WelcomeWidget::slotSelectionChanged() {
	this->updateToolButtonsEnabledState();
}

ot::ToolButton* WelcomeWidget::iniToolButton(const QString& _text, const QString& _iconPath, tt::Group* _group, const QString& _toolTip) {
	ot::ToolButton* newButton = new ot::ToolButton(ot::IconManager::getIcon(_iconPath), _text, _group);
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

void WelcomeWidget::updateToolButtonsEnabledState(bool _forceDisabled) {
	std::list<ot::ProjectInformation> selectedProjects = m_overview->getSelectedProjects();

	bool hasDifferentOwner = false;
	for (const ot::ProjectInformation& proj : selectedProjects) {
		if (proj.getUserName() != AppBase::instance()->getCurrentLoginData().getUserName()) {
			hasDifferentOwner = true;
			break;
		}
	}

	if (selectedProjects.empty() || _forceDisabled) {
		m_openButton->setEnabled(false);
		m_copyButton->setEnabled(false);
		m_renameButton->setEnabled(false);
		m_deleteButton->setEnabled(false);
		m_exportButton->setEnabled(false);
		m_accessButton->setEnabled(false);
		m_ownerButton->setEnabled(false);
	}
	else if (selectedProjects.size() == 1) {
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
