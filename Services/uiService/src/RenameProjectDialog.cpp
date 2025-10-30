// @otlicense
// File: RenameProjectDialog.cpp
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
#include "RenameProjectDialog.h"
#include "ProjectManagement.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qmessagebox.h>

RenameProjectDialog::RenameProjectDialog(const QString& _projectToRename, ProjectManagement& projectManager)
	: m_projectToRename(_projectToRename), m_projectManager(&projectManager) {
	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);

	QHBoxLayout* inputLayout = new QHBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	// Create controls
	ot::Label* nameLabel = new ot::Label("New Name:");
	m_edit = new ot::LineEdit(m_projectToRename);

	ot::PushButton* confirmButton = new ot::PushButton("Confirm");
	ot::PushButton* cancelButton = new ot::PushButton("Cancel");

	// Setup layouts
	centralLayout->addLayout(inputLayout);
	centralLayout->addLayout(buttonLayout);
	centralLayout->addStretch(1);

	inputLayout->addWidget(nameLabel);
	inputLayout->addWidget(m_edit, 1);

	buttonLayout->addStretch(1);
	buttonLayout->addWidget(confirmButton);
	buttonLayout->addWidget(cancelButton);

	// Setup dialog
	this->setWindowTitle("Rename project (" + m_projectToRename + ")");
	this->setWindowIcon(ot::IconManager::getApplicationIcon());
	this->setMinimumWidth(200);

	// Connect signals
	this->connect(confirmButton, &ot::PushButton::clicked, this, &RenameProjectDialog::slotConfirm);
	this->connect(cancelButton, &ot::PushButton::clicked, this, &RenameProjectDialog::closeCancel);
}

RenameProjectDialog::~RenameProjectDialog() {

}

QString RenameProjectDialog::getProjectName(void) const { return m_edit->text(); }

void RenameProjectDialog::slotConfirm() {
	if (this->getProjectName() == m_projectToRename) {
		QMessageBox msg(QMessageBox::Warning, "Invalid Name", "The new project name must be different from the origin name. Please choose a different name.", QMessageBox::Ok, m_edit);
		msg.exec();
		return;
	}

	bool canBeDeleted = false;
	if (m_projectManager->projectExists(this->getProjectName().toStdString(), canBeDeleted))
	{
		QMessageBox msg(QMessageBox::Warning, "Invalid Name", "A project with the new name already exists.\nPlease choose another unique name.", QMessageBox::Ok, m_edit);
		msg.exec();
		return;
	}
	
	this->closeOk();
}
