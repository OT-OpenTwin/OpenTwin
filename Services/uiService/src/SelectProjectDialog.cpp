// @otlicense

// Frontend header
#include "AppBase.h"
#include "ProjectManagement.h"
#include "SelectProjectDialog.h"
#include "ProjectOverviewWidget.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlistwidget.h>

SelectProjectDialog::SelectProjectDialog(const ot::DialogCfg& _config)
	: Dialog(_config)
{
	// Setup widgets
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QVBoxLayout* centralLayout = new QVBoxLayout;
	QHBoxLayout* modeLayout = new QHBoxLayout;
	QHBoxLayout* bottomLayout = new QHBoxLayout;

	modeLayout = new QHBoxLayout;
	centralLayout->addLayout(modeLayout);

	m_modeSelector = new ot::ComboBox(this);
	m_modeSelector->setEditable(false);
	m_modeSelector->addItem("All Projects", QVariant((int)ot::ProjectOverviewWidget::AllMode));
	m_modeSelector->addItem("Recent Projects", QVariant((int)ot::ProjectOverviewWidget::RecentMode));
	m_modeSelector->setCurrentIndex(0);
	modeLayout->addWidget(new ot::Label("Mode:", this));
	modeLayout->addWidget(m_modeSelector);
	modeLayout->addStretch(1);
	connect(m_modeSelector, &ot::ComboBox::currentIndexChanged, this, &SelectProjectDialog::slotModeChanged);

	m_overview = new ot::ProjectOverviewWidget(this);
	m_overview->setMultiSelectionEnabled(false);
	m_overview->refreshAllProjects();
	centralLayout->addWidget(m_overview->getQWidget());

	mainLayout->addLayout(centralLayout, 1);

	m_infoLabel = new ot::Label(this);

	m_confirmButton = new ot::PushButton("Confirm", this);
	m_confirmButton->setEnabled(false);
	ot::PushButton* cancelButton = new ot::PushButton("Cancel", this);

	bottomLayout->addWidget(m_infoLabel, 1);
	bottomLayout->addWidget(m_confirmButton);
	bottomLayout->addWidget(cancelButton);
	mainLayout->addLayout(bottomLayout);

	connect(m_confirmButton, &QPushButton::clicked, this, &SelectProjectDialog::slotConfirm);
	connect(cancelButton, &QPushButton::clicked, this, &SelectProjectDialog::closeCancel);
	connect(m_overview, &ot::ProjectOverviewWidget::selectionChanged, this, &SelectProjectDialog::slotSelectionChanged);
	connect(m_overview, &ot::ProjectOverviewWidget::projectOpenRequested, this, &SelectProjectDialog::slotOpenRequested);
}

SelectProjectDialog::~SelectProjectDialog() {
}

ot::ProjectInformation SelectProjectDialog::getSelectedProject() const {
	auto lst = m_overview->getSelectedProjects();
	if (lst.empty()) {
		OT_LOG_E("No project selected");
		return ot::ProjectInformation();
	}
	else {
		return lst.front();
	}
}

void SelectProjectDialog::slotConfirm() {
	auto sel = m_overview->getSelectedProjects();
	if (sel.size() != 1) {
		return;
	}
	else {
		this->closeOk();
	}
}

void SelectProjectDialog::slotOpenRequested() {
	this->slotConfirm();
}

void SelectProjectDialog::slotSelectionChanged() {
	m_confirmButton->setEnabled(!m_overview->getSelectedProjects().empty());
}

void SelectProjectDialog::slotModeChanged() {
	ot::ProjectOverviewWidget::DataMode mode = static_cast<ot::ProjectOverviewWidget::DataMode>(m_modeSelector->currentData().toInt());
	switch (mode) {
	case ot::ProjectOverviewWidget::RecentMode:
		m_overview->refreshRecentProjects();
		break;
	case ot::ProjectOverviewWidget::AllMode:
		m_overview->refreshAllProjects();
		break;
	default:
		OT_LOG_E("Unknown project mode (" + std::to_string(mode) + ")");
		break;
	}
}
