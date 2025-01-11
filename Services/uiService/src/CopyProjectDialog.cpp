//! @file CopyProjectDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

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

CopyProjectDialog::CopyProjectDialog(const QString& _projectToCopy, ProjectManagement& projectManager)
	: m_projectToCopy(_projectToCopy),
	  m_projectManagement(&projectManager)
{
	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);

	QHBoxLayout* inputLayout = new QHBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	// Create controls
	ot::Label* nameLabel = new ot::Label("New Name:");
	m_edit = new ot::LineEdit(m_projectToCopy + " - copy");

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
	this->setWindowTitle("Copy project (" + m_projectToCopy + ")");
	this->setWindowIcon(ot::IconManager::getApplicationIcon());
	this->setMinimumWidth(200);

	// Connect signals
	this->connect(confirmButton, &ot::PushButton::clicked, this, &CopyProjectDialog::slotConfirm);
	this->connect(cancelButton, &ot::PushButton::clicked, this, &CopyProjectDialog::closeCancel);
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
