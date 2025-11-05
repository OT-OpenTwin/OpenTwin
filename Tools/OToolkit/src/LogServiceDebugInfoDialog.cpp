// @otlicense
// File: LogServiceDebugInfoDialog.cpp
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

// OToolkit header
#include "LogServiceDebugInfoDialog.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PlainTextEdit.h"

// Qt header
#include <QtCore/qjsondocument.h>
#include <QtWidgets/qlayout.h>

LogServiceDebugInfoDialog::LogServiceDebugInfoDialog(const QString& _name, const QString& _url, const QString& _id, const QString& _pid, const QString& _info, QWidget* _parent)
	: ot::Dialog(_parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QGridLayout* grid = new QGridLayout;
	grid->addWidget(new ot::Label("Name:", this), 0, 0);
	ot::LineEdit* nameEdit = new ot::LineEdit(_name, this);
	nameEdit->setReadOnly(true);
	grid->addWidget(nameEdit, 0, 1);

	grid->addWidget(new ot::Label("URL:", this), 1, 0);
	ot::LineEdit* urlEdit = new ot::LineEdit(_url, this);
	urlEdit->setReadOnly(true);
	grid->addWidget(urlEdit, 1, 1);

	grid->addWidget(new ot::Label("ID:", this), 2, 0);
	ot::LineEdit* idEdit = new ot::LineEdit(_id, this);
	idEdit->setReadOnly(true);
	grid->addWidget(idEdit, 2, 1);

	grid->addWidget(new ot::Label("PID:", this), 3, 0);
	ot::LineEdit* pidEdit = new ot::LineEdit(_pid, this);
	pidEdit->setReadOnly(true);
	grid->addWidget(pidEdit, 3, 1);

	mainLayout->addLayout(grid);

	QVBoxLayout* infoLayout = new QVBoxLayout;
	infoLayout->addWidget(new ot::Label("Additional Information:", this));
	ot::PlainTextEdit* infoEdit = new ot::PlainTextEdit(this);
	infoEdit->setReadOnly(true);
	if (_info.isEmpty()) {
		infoEdit->setPlaceholderText("No additional information available");
	}
	else {
		QJsonParseError parseError;
		QJsonDocument doc = QJsonDocument::fromJson(_info.toUtf8(), &parseError);
		if (parseError.error == QJsonParseError::NoError) {
			infoEdit->setPlainText(doc.toJson(QJsonDocument::Indented));
		}
		else {
			infoEdit->setPlainText(_info);
		}
	}
	infoLayout->addWidget(infoEdit);
	mainLayout->addLayout(infoLayout, 1);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	ot::PushButton* closeButton = new ot::PushButton("Close", this);
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(closeButton, 0);
	mainLayout->addLayout(buttonLayout);
	this->connect(closeButton, &ot::PushButton::clicked, this, &LogServiceDebugInfoDialog::closeOk);

	this->setWindowTitle("Service Information - " + _name);
	this->setWindowIcon(ot::IconManager::getApplicationIcon());
	this->resize(400, 600);
}
