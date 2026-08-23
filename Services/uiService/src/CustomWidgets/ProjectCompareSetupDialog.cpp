// @otlicense

// Frontend header
#include "AppBase.h"
#include "ProjectManagement.h"
#include "CustomWidgets/ProjectCompareSetupDialog.h"
#include "ProjectOverview/ProjectOverviewWidget.h"

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Version/VersionGraph.h"
#include "OTWidgets/Version/VersionGraphManager.h"
#include "OTWidgets/Widgets/Label.h"
#include "OTWidgets/Widgets/CheckBox.h"
#include "OTWidgets/Widgets/PushButton.h"
#include "OTCommunication/ActionTypes.h"

// Qt header
#include <QtWidgets/qlayout.h>

using namespace ot;

ProjectCompareSetupDialog::ProjectCompareSetupDialog(const DialogCfg& _config, QWidget* _parent)
	: Dialog(_config, _parent), m_state(DialogState::ProjectSelection)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	m_viewLayout = new QHBoxLayout;
	mainLayout->addLayout(m_viewLayout, 1);

	// Project

	m_projectOverview = new ProjectOverviewWidget(this);
	m_projectOverview->setMultiSelectionEnabled(false);
	m_viewLayout->addWidget(m_projectOverview, 1);
	
	connect(m_projectOverview, &ProjectOverviewWidget::selectionChanged, this, [this]() 
		{
		auto selection = m_projectOverview->getSelectedProjects();
		if (selection.empty()) {
			m_selectedProject = ProjectInformation();
			updateButtonStates();
		}
		else if (selection.size() > 1)
		{
			OT_LOG_E("Invalid selection");
		}
		else
		{
			m_selectedProject = std::move(selection.front());
		}
		updateButtonStates();
		}
	);

	connect(m_projectOverview, &ProjectOverviewWidget::projectOpenRequested, this, [this](const ProjectInformation& _project) {
		m_selectedProject = _project;
		QMetaObject::invokeMethod(this, &ProjectCompareSetupDialog::slotNext, Qt::QueuedConnection);
		}
	);

	// Version Graph

	m_versionGraph = new VersionGraphManager(nullptr);
	m_versionGraph->getQWidget()->hide();
	connect(m_versionGraph->getGraph(), &VersionGraph::versionSelected, this, [this](const std::string& _version) {
		m_selectedVersion = _version;
		updateButtonStates();
		}
	);

	connect(m_versionGraph->getGraph(), &VersionGraph::versionActivateRequest, this, [this](const std::string& _version) {
		m_selectedVersion = _version;
		m_versionGraph->getGraph()->selectVersion(_version);
		updateButtonStates();
		QMetaObject::invokeMethod(this, &ProjectCompareSetupDialog::slotNext, Qt::QueuedConnection);
		}
	);
	
	// Summary

	m_summaryWidget.root = new QWidget(nullptr);
	m_summaryWidget.root->hide();

	QVBoxLayout* summaryLayout = new QVBoxLayout(m_summaryWidget.root);
	QGridLayout* summaryInfoLayout = new QGridLayout;
	summaryLayout->addLayout(summaryInfoLayout);

	summaryInfoLayout->addWidget(new Label("Comparing", m_summaryWidget.root), 0, 0);
	m_summaryWidget.sourceProjectVersionLabel = new Label("    ", m_summaryWidget.root);
	QFont font = m_summaryWidget.sourceProjectVersionLabel->font();
	font.setItalic(true);
	m_summaryWidget.sourceProjectVersionLabel->setFont(font);
	summaryInfoLayout->addWidget(m_summaryWidget.sourceProjectVersionLabel, 1, 0);
	m_summaryWidget.sourceProjectLabel = new Label("", m_summaryWidget.root);
	summaryInfoLayout->addWidget(m_summaryWidget.sourceProjectLabel, 1, 1);
	summaryInfoLayout->addWidget(new Label("with", m_summaryWidget.root), 2, 0);
	m_summaryWidget.targetProjectVersionLabel = new Label("    ", m_summaryWidget.root);
	m_summaryWidget.targetProjectVersionLabel->setFont(font);
	summaryInfoLayout->addWidget(m_summaryWidget.targetProjectVersionLabel, 3, 0);
	m_summaryWidget.targetProjectLabel = new Label("", m_summaryWidget.root);
	summaryInfoLayout->addWidget(m_summaryWidget.targetProjectLabel, 3, 1);
	
	summaryInfoLayout->setColumnStretch(0, 0);
	summaryInfoLayout->setColumnStretch(1, 1);

	QGridLayout* summaryOptionsLayout = new QGridLayout;
	summaryLayout->addLayout(summaryOptionsLayout);

	int rowIx = 0;

	summaryOptionsLayout->addWidget(new Label("Compare Properties", m_summaryWidget.root), rowIx, 0);
	m_summaryWidget.parseProperties = new CheckBox(m_summaryWidget.root);
	summaryOptionsLayout->addWidget(m_summaryWidget.parseProperties, rowIx++, 1);
	
	summaryOptionsLayout->addWidget(new Label("Compare Data", m_summaryWidget.root), rowIx, 0);
	m_summaryWidget.parseData = new CheckBox(m_summaryWidget.root);
	connect(m_summaryWidget.parseData, &CheckBox::stateChanged, this, &ProjectCompareSetupDialog::slotParseDataChanged);
	summaryOptionsLayout->addWidget(m_summaryWidget.parseData, rowIx++, 1);

	summaryOptionsLayout->addWidget(new Label("Compare Results", m_summaryWidget.root), rowIx, 0);
	m_summaryWidget.parseResults = new CheckBox(m_summaryWidget.root);
	m_summaryWidget.parseResults->setEnabled(m_summaryWidget.previousParseResults);
	connect(m_summaryWidget.parseResults, &CheckBox::stateChanged, this, &ProjectCompareSetupDialog::slotParseResultsChanged);
	summaryOptionsLayout->addWidget(m_summaryWidget.parseResults, rowIx++, 1);

	summaryOptionsLayout->setColumnStretch(0, 0);
	summaryOptionsLayout->setColumnStretch(1, 1);

	summaryLayout->addStretch(1);

	// Buttons

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	mainLayout->addLayout(buttonLayout);

	m_backButton = new PushButton("Back", this);
	connect(m_backButton, &PushButton::clicked, this, &ProjectCompareSetupDialog::slotBack);
	buttonLayout->addWidget(m_backButton);

	m_nextButton = new PushButton("Next", this);
	connect(m_nextButton, &PushButton::clicked, this, &ProjectCompareSetupDialog::slotNext);
	buttonLayout->addWidget(m_nextButton);

	buttonLayout->addStretch(1);

	m_confirmButton = new PushButton("Confirm", this);
	connect(m_confirmButton, &PushButton::clicked, this, &ProjectCompareSetupDialog::slotConfirm);
	buttonLayout->addWidget(m_confirmButton);

	refreshProjects();
	updateButtonStates();
}

ProjectCompareConfig ProjectCompareSetupDialog::getCompareConfig() const
{
	ProjectCompareConfig cfg;

	cfg.setTargetProjectName(m_selectedProject.getProjectName());
	cfg.setTargetProjectVersion(m_selectedVersion);

	cfg.setFlag(ProjectCompareConfig::ProjectCompareFlag::CompareProperties, getParseProperties());
	cfg.setFlag(ProjectCompareConfig::ProjectCompareFlag::CompareData, getParseData());
	cfg.setFlag(ProjectCompareConfig::ProjectCompareFlag::CompareResults, getParseResults());

	return cfg;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public slots

void ProjectCompareSetupDialog::refreshProjects()
{
	m_selectedProject = ProjectInformation();
	m_selectedVersion.clear();

	m_projectOverview->refreshProjects();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void ProjectCompareSetupDialog::slotBack() {
	switch (m_state)
	{
	case DialogState::ProjectSelection:  OT_LOG_E("Invalid button state");  break;
	case DialogState::VersionSelection:  switchToProjectSelection();        break;
	case DialogState::Summary:           switchToVersionSelection();        break;
	default:
		OT_LOG_ES("Unknown dialog state (" << (int)m_state << ")");
		break;
	}
	
	updateButtonStates();
}

void ProjectCompareSetupDialog::slotNext()
{
	switch (m_state)
	{
	case DialogState::ProjectSelection:  switchToVersionSelection();        break;
	case DialogState::VersionSelection:  switchToSummary();                 break;
	case DialogState::Summary:           OT_LOG_E("Invalid button state");  break;
	default:
		OT_LOG_ES("Unknown dialog state (" << (int)m_state << ")");
		break;
	}

	updateButtonStates();
}

void ProjectCompareSetupDialog::slotConfirm()
{
	if (m_state != DialogState::Summary)
	{
		OT_LOG_E("Invalid button state");
		return;
	}
	
	closeOk();
}

void ProjectCompareSetupDialog::updateButtonStates()
{
	switch (m_state)
	{
	case ProjectCompareSetupDialog::DialogState::ProjectSelection:
		m_backButton->setEnabled(false);
		m_nextButton->setEnabled(!m_selectedProject.getProjectName().empty());
		m_confirmButton->setEnabled(false);
		break;
	case ProjectCompareSetupDialog::DialogState::VersionSelection:
		m_backButton->setEnabled(true);
		m_nextButton->setEnabled(!m_selectedVersion.empty());
		m_confirmButton->setEnabled(false);
		break;
	case ProjectCompareSetupDialog::DialogState::Summary:
		m_backButton->setEnabled(true);
		m_nextButton->setEnabled(false);
		m_confirmButton->setEnabled(true);
		break;
	default:
		OT_LOG_ES("Unknown dialog state (" << (int)m_state << ")");
		break;
	}
}

void ProjectCompareSetupDialog::refreshVersionGraph()
{
	m_selectedVersion.clear();

	if (m_selectedProject.getProjectName().empty())
	{
		OT_LOG_E("No project selected");
		return;
	}

	AppBase* app = AppBase::instance();

	JsonDocument doc;
	JsonAllocator& alloc = doc.GetAllocator();
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_MODEL_GetVersionGraph, alloc), alloc);
	doc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, JsonString(m_selectedProject.getProjectName(), alloc), alloc);

	std::string responseStr;
	app->sendExecuteToService(BasicServiceInformation(OT_INFO_SERVICE_TYPE_MODEL), doc.toJson(), responseStr);

	ReturnMessage response = ReturnMessage::fromJson(responseStr);
	if (!response.isOk())
	{
		OT_LOG_ES("Failed to get version graph for project \"" << m_selectedProject.getProjectName() << "\": " << response.getWhat());
		return;
	}

	JsonDocument cfgDoc;
	cfgDoc.fromJson(response.getWhat());

	VersionGraphCfg cfg;
	cfg.setFromJsonObject(cfgDoc.getConstObject());

	m_versionGraph->setupConfig(std::move(cfg));

	updateButtonStates();
}

void ProjectCompareSetupDialog::updateSummary()
{
	const std::string sourceProjectName = getSourceProjectName();
	const std::string sourceProjectVersion = getSourceProjectVersion();

	const std::string targetProjectName = getTargetProjectName();
	const std::string targetProjectVersion = getTargetProjectVersion();

	m_summaryWidget.sourceProjectVersionLabel->setText(QString::fromStdString("    " + (sourceProjectVersion.empty() ? std::string() : "(version: " + sourceProjectVersion + ")")));
	m_summaryWidget.sourceProjectLabel->setText(QString::fromStdString(sourceProjectName));

	m_summaryWidget.targetProjectVersionLabel->setText(QString::fromStdString("    " + (targetProjectVersion.empty() ? std::string() : "(version: " + targetProjectVersion + ")")));
	m_summaryWidget.targetProjectLabel->setText(QString::fromStdString(targetProjectName));
}

void ProjectCompareSetupDialog::slotParseDataChanged()
{
	if (!getParseData())
	{
		m_summaryWidget.previousParseResults = m_summaryWidget.parseResults->isChecked();
		m_summaryWidget.parseResults->setChecked(false);
		m_summaryWidget.parseResults->setEnabled(false);
	}
	else
	{
		m_summaryWidget.parseResults->setEnabled(true);
		if (m_summaryWidget.previousParseResults)
		{
			m_summaryWidget.parseResults->setChecked(true);
		}
	}
}

void ProjectCompareSetupDialog::slotParseResultsChanged()
{
	if (getParseData())
	{
		m_summaryWidget.previousParseResults = getParseResults();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private data accessors

bool ProjectCompareSetupDialog::getParseProperties() const
{
	return m_summaryWidget.parseProperties->isChecked();
}

bool ProjectCompareSetupDialog::getParseData() const
{
	return m_summaryWidget.parseData->isChecked();
}

bool ProjectCompareSetupDialog::getParseResults() const
{
	return getParseData() && m_summaryWidget.parseResults->isChecked();
}

std::string ProjectCompareSetupDialog::getSourceProjectName() const
{
	AppBase* app = AppBase::instance();
	return app->getCurrentProjectInfo().getProjectName();
}

std::string ProjectCompareSetupDialog::getSourceProjectVersion() const
{
	AppBase* app = AppBase::instance();
	auto version = app->getActiveProjectVersion();
	if (version.has_value())
	{
		return version.value();
	}
	else
	{
		return "";
	}
}

std::string ProjectCompareSetupDialog::getTargetProjectName() const
{
	return m_selectedProject.getProjectName();
}

std::string ProjectCompareSetupDialog::getTargetProjectVersion() const
{
	return m_selectedVersion;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private helper

QWidget* ProjectCompareSetupDialog::getCurrentViewWidget()
{
	QWidget* widget = nullptr;

	switch (m_state)
	{
	case DialogState::ProjectSelection: widget = m_projectOverview->getQWidget(); break;
	case DialogState::VersionSelection: widget = m_versionGraph->getQWidget(); break;
	case DialogState::Summary: widget = m_summaryWidget.root; break;
	default:
		OT_LOG_ES("Unknown dialog state (" << (int)m_state << ")");
		break;
	}

	return widget;
}

void ProjectCompareSetupDialog::unsetView()
{
	QWidget* currentView = getCurrentViewWidget();
	if (currentView)
	{
		m_viewLayout->removeWidget(currentView);
		currentView->setParent(nullptr);
		currentView->hide();
	}
}

void ProjectCompareSetupDialog::switchToProjectSelection()
{
	if (m_state == DialogState::ProjectSelection)
	{
		return;
	}

	unsetView();

	m_state = DialogState::ProjectSelection;

	m_viewLayout->addWidget(m_projectOverview->getQWidget(), 1);
	m_projectOverview->getQWidget()->setHidden(false);

	updateButtonStates();
}

void ProjectCompareSetupDialog::switchToVersionSelection()
{
	if (m_state == DialogState::VersionSelection)
	{
		return;
	}

	unsetView();

	m_state = DialogState::VersionSelection;

	refreshVersionGraph();

	m_viewLayout->addWidget(m_versionGraph->getQWidget(), 1);
	m_versionGraph->getQWidget()->setHidden(false);

	updateButtonStates();
}

void ProjectCompareSetupDialog::switchToSummary()
{
	if (m_state == DialogState::Summary)
	{
		return;
	}

	unsetView();

	m_state = DialogState::Summary;

	updateSummary();

	m_viewLayout->addWidget(m_summaryWidget.root, 1);
	m_summaryWidget.root->setHidden(false);

	updateButtonStates();
}
