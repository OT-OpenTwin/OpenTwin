//! @file ProjectOverviewWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

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

WelcomeWidget::WelcomeWidget(tt::Page* _ttbPage) {
	// Create layouts
	m_widget = new QWidget;
	QVBoxLayout* centralLayout = new QVBoxLayout(m_widget);

	// Crete controls
	m_titleLabel = new ot::Label("Projects");
	m_filter = new ot::LineEdit;
	m_countLabel = new ot::Label;
	QOpenGLWidget* glWidget = new QOpenGLWidget;

	m_overview = new ot::ProjectOverviewWidget;
	m_overview->setMultiSelectionEnabled(true);

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

	glWidget->setMaximumSize(1, 1);

	this->updateCountLabel();
	this->updateToggleViewModeButton();
	this->updateToolButtonsEnabledState();

	// Setup layouts
	centralLayout->addWidget(m_titleLabel);
	centralLayout->addWidget(m_filter);
	centralLayout->addWidget(m_overview->getQWidget());
	centralLayout->addWidget(m_countLabel);
	centralLayout->addWidget(glWidget);
	
	slotRefreshRecentProjects();

	// Connect signals
	this->connect(m_filter, &ot::LineEdit::textChanged, this, &WelcomeWidget::slotFilterChanged);
	this->connect(m_overview, &ot::ProjectOverviewWidget::selectionChanged, this, &WelcomeWidget::slotSelectionChanged);
	this->connect(m_overview, &ot::ProjectOverviewWidget::projectOpenRequested, this, &WelcomeWidget::slotOpenProject);
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
	if (_watched == m_filter && _event->type() == QEvent::KeyPress) {
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

QString WelcomeWidget::getGeneralFilter() const {
	return m_filter->text();
}

std::list<ot::ProjectInformation> WelcomeWidget::getSelectedProjects() const {
	return m_overview->getSelectedProjects();
}

void WelcomeWidget::slotCreateProject() {
	Q_EMIT createProjectRequest();
}

void WelcomeWidget::slotRefreshProjectList() {
	m_overview->refreshProjectList();

	this->updateCountLabel();
	this->updateToggleViewModeButton();
	this->updateToolButtonsEnabledState();
}

void WelcomeWidget::slotRefreshRecentProjects() {
	m_titleLabel->setText("Recent Projects");

	m_overview->refreshRecentProjects();

	this->updateCountLabel();
	this->updateToggleViewModeButton();
	this->updateToolButtonsEnabledState();
}

void WelcomeWidget::slotRefreshAllProjects() {
	m_titleLabel->setText("Projects");

	m_overview->refreshAllProjects();

	this->updateCountLabel();
	this->updateToggleViewModeButton();
	this->updateToolButtonsEnabledState();
}

void WelcomeWidget::slotToggleViewMode() {
	switch (m_overview->getDataMode()) {
	case ot::ProjectOverviewWidget::RecentMode:
		slotRefreshAllProjects();
		break;
	case ot::ProjectOverviewWidget::AllMode:
		slotRefreshRecentProjects();
		break;
	default:
		OT_LOG_EAS("Unknown view mode (" + std::to_string(static_cast<int>(m_overview->getDataMode())) + ")");
		break;
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

void WelcomeWidget::slotFilterChanged() {
	m_overview->setGeneralFilter(m_filter->text());
}

void WelcomeWidget::slotSelectionChanged() {
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

void WelcomeWidget::updateCountLabel() {
	int count = m_overview->getProjectCount();

	if (count == 0) {
		m_countLabel->setText("No projects found");
	}
	else if (count == 1) {
		if (m_overview->getProjectsReultsExceeded()) {
			m_countLabel->setText("Showing the first project found");
		}
		else {
			m_countLabel->setText("1 project found");
		}
	}
	else {
		if (m_overview->getProjectsReultsExceeded()) {
			m_countLabel->setText("Showing the first " + QString::number(count) + " projects");
		}
		else {
			m_countLabel->setText(QString::number(count) + " projects found");
		}
	}
}

void WelcomeWidget::updateToggleViewModeButton() {
	switch (m_overview->getDataMode()) {
	case ot::ProjectOverviewWidget::AllMode:
		m_toggleViewModeButton->setText("View Recent");
		m_toggleViewModeButton->setIcon(ot::IconManager::getIcon("ToolBar/ViewRecentProjects.png"));
		m_toggleViewModeButton->setToolTip("View recent projects (Currently showing all projects)");
		break;
	case ot::ProjectOverviewWidget::RecentMode:
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
	int count = m_overview->getSelectedProjects().size();
	bool hasDifferentOwner = m_overview->hasOtherUser(AppBase::instance()->getCurrentLoginData().getUserName());

	if (count == 0 || _forceDisabled) {
		m_openButton->setEnabled(false);
		m_copyButton->setEnabled(false);
		m_renameButton->setEnabled(false);
		m_deleteButton->setEnabled(false);
		m_exportButton->setEnabled(false);
		m_accessButton->setEnabled(false);
		m_ownerButton->setEnabled(false);
	}
	else if (count == 1) {
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
