// @otlicense
// File: CommitDialog.cpp
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

// OpenTwin header
#include "OTFMC/CommitDialog.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ToolTipHandler.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::CommitDialog::CommitDialog(QWidget* _parent)
	: Dialog(_parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* messageLayout = new QHBoxLayout;
	mainLayout->addLayout(messageLayout, 1);

	messageLayout->addWidget(new Label("Commit Message:", this));
	m_commitMessage = new LineEdit(this);
	m_commitMessage->setMaxLength(256);
	messageLayout->addWidget(m_commitMessage, 1);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch(1);
	mainLayout->addLayout(buttonLayout);

	ot::PushButton* confirmButton = new ot::PushButton("Confirm", this);
	confirmButton->setDefault(true);
	buttonLayout->addWidget(confirmButton);
	connect(confirmButton, &ot::PushButton::clicked, this, &CommitDialog::slotConfirm);

	ot::PushButton* cancelButton = new ot::PushButton("Cancel", this);
	buttonLayout->addWidget(cancelButton);
	connect(cancelButton, &ot::PushButton::clicked, this, &CommitDialog::closeCancel);

	this->setWindowTitle("Commit Changes");
	this->setMinimumSize(400, 100);
	this->resize(500, 100);
}

ot::CommitDialog::~CommitDialog() {

}

void ot::CommitDialog::slotConfirm() {
	if (m_commitMessage->text().isEmpty()) {
		QPoint pos = m_commitMessage->mapToGlobal(m_commitMessage->rect().bottomLeft());
		ToolTipHandler::showToolTip(pos, "Please enter a commit message.");
		return;
	}

	closeOk();
}
