//! @file LogServiceDebugInfoDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

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

LogServiceDebugInfoDialog::LogServiceDebugInfoDialog(const QString& _name, const QString& _url, const QString& _id, const QString& _info, QWidget* _parent)
	: ot::Dialog(_parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QGridLayout* grid = new QGridLayout;
	grid->addWidget(new ot::Label("Name:"), 0, 0);
	ot::LineEdit* nameEdit = new ot::LineEdit(_name);
	nameEdit->setReadOnly(true);
	grid->addWidget(nameEdit, 0, 1);

	grid->addWidget(new ot::Label("URL:"), 1, 0);
	ot::LineEdit* urlEdit = new ot::LineEdit(_url);
	urlEdit->setReadOnly(true);
	grid->addWidget(urlEdit, 1, 1);

	grid->addWidget(new ot::Label("ID:"), 2, 0);
	ot::LineEdit* idEdit = new ot::LineEdit(_id);
	idEdit->setReadOnly(true);
	grid->addWidget(idEdit, 2, 1);

	mainLayout->addLayout(grid);

	QVBoxLayout* infoLayout = new QVBoxLayout;
	infoLayout->addWidget(new ot::Label("Additional Information:"));
	ot::PlainTextEdit* infoEdit = new ot::PlainTextEdit;
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
	ot::PushButton* closeButton = new ot::PushButton("Close");
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(closeButton, 0);
	mainLayout->addLayout(buttonLayout);
	this->connect(closeButton, &ot::PushButton::clicked, this, &LogServiceDebugInfoDialog::closeOk);

	this->setWindowTitle("Service Information - " + _name);
	this->setWindowIcon(ot::IconManager::getApplicationIcon());
	this->resize(400, 600);
}
