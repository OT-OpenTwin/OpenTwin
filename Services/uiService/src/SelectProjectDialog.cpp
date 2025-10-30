// @otlicense
// File: SelectProjectDialog.cpp
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
#include "ProjectManagement.h"
#include "SelectProjectDialog.h"
#include "ProjectOverviewWidget.h"

// OpenTwin header
#include "OTWidgets/Label.h"
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
	QHBoxLayout* bottomLayout = new QHBoxLayout;

	m_overview = new ot::ProjectOverviewWidget(this);
	m_overview->setMultiSelectionEnabled(false);
	m_overview->refreshProjects();
	mainLayout->addWidget(m_overview->getQWidget(), 1);

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
