// @otlicense
// File: CopyProjectDialog.cpp
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
#include "CopyProjectDialog.h"
#include "ProjectManagement.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qmessagebox.h>

CopyProjectDialog::CopyProjectDialog(const QString& _projectToCopy, ProjectManagement& projectManager, QWidget* _parent)
	: ot::Dialog(_parent), m_projectToCopy(_projectToCopy), m_projectManagement(&projectManager)
{
	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);

	QHBoxLayout* inputLayout = new QHBoxLayout;
	centralLayout->addLayout(inputLayout);
	
	ot::Label* nameLabel = new ot::Label("New Name:", this);
	inputLayout->addWidget(nameLabel);

	m_edit = new ot::LineEdit(m_projectToCopy + " - copy", this);
	inputLayout->addWidget(m_edit, 1);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch(1);
	centralLayout->addLayout(buttonLayout);

	ot::PushButton* confirmButton = new ot::PushButton("Confirm", this);
	buttonLayout->addWidget(confirmButton);
	this->connect(confirmButton, &ot::PushButton::clicked, this, &CopyProjectDialog::slotConfirm);

	ot::PushButton* cancelButton = new ot::PushButton("Cancel", this);
	buttonLayout->addWidget(cancelButton);
	this->connect(cancelButton, &ot::PushButton::clicked, this, &CopyProjectDialog::closeCancel);

	centralLayout->addStretch(1);
	
	// Setup dialog
	this->setWindowTitle("Copy project (" + m_projectToCopy + ")");
	this->setWindowIcon(ot::IconManager::getApplicationIcon());
	this->setMinimumWidth(200);
}

CopyProjectDialog::~CopyProjectDialog() {

}

QString CopyProjectDialog::getProjectName(void) const { return m_edit->text(); }

void CopyProjectDialog::slotConfirm() { 
	if (this->getProjectName() == m_projectToCopy) {
		QMessageBox msg(QMessageBox::Warning, "Invalid Name", "The new project name must be different from the origin name. Please choose a different name.", QMessageBox::Ok, m_edit);
		msg.exec();
		return;
	}

	bool canBeDeleted = false;
	if (m_projectManagement->projectExists(this->getProjectName().toStdString(), canBeDeleted))
	{
		if (canBeDeleted)
		{
			QMessageBox msg(QMessageBox::Warning, "Project already exists", "A project with the new name already exists. Do you want to overwrite the existing project?\n\n"
																		    "Please note that this operation cannot be undone!", QMessageBox::Yes | QMessageBox::Cancel, m_edit);
			if (msg.exec() == QMessageBox::Yes)
			{
				this->closeOk();
			}
			else
			{
				return;
			}
		}
		else
		{
			QMessageBox msg(QMessageBox::Warning, "Project already exists", "A project with the new name already exists and cannot be overwritten. \nPlease choose another name for the new project.", QMessageBox::Ok, m_edit);
			msg.exec();
			return;
		}
	}

	this->closeOk();
}
