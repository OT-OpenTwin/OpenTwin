// @otlicense
// File: OnePropertyDialog.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTGui/Property.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/OnePropertyDialog.h"
#include "OTWidgets/PropertyInputFactory.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qpushbutton.h>

ot::OnePropertyDialog::OnePropertyDialog(const OnePropertyDialogCfg& _config, QWidget* _parent)
	: Dialog(_config, _parent), m_changed(false)
{
	m_input = PropertyInputFactory::createInput(_config.getProperty());

	// Create layouts
	QVBoxLayout* cLay = new QVBoxLayout(this);
	QHBoxLayout* inLay = new QHBoxLayout;
	QHBoxLayout* btnLay = new QHBoxLayout;

	// Create controls
	QLabel* l = new QLabel(QString::fromStdString(_config.getProperty()->getPropertyTitle()));
	QPushButton* btnConfirm = new QPushButton("Confirm");
	QPushButton* btnCancel = new QPushButton("Cancel");

	// Setup layouts
	cLay->addLayout(inLay);
	cLay->addStretch(1);
	cLay->addLayout(btnLay);

	inLay->addWidget(l);
	inLay->addWidget(m_input->getQWidget(), 1);

	btnLay->addStretch(1);
	btnLay->addWidget(btnConfirm);
	btnLay->addWidget(btnCancel);

	// Connect signals
	this->connect(m_input, &PropertyInput::inputValueChanged, this, &OnePropertyDialog::slotValueChanged);
	this->connect(btnConfirm, &QPushButton::clicked, this, &OnePropertyDialog::slotConfirm);
	this->connect(btnCancel, &QPushButton::clicked, this, &OnePropertyDialog::closeCancel);
}

ot::OnePropertyDialog::~OnePropertyDialog() {
	if (m_input) delete m_input;
}

void ot::OnePropertyDialog::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	if (m_input) {
		m_input->addPropertyInputValueToJson(_object, _memberNameValue, _allocator);
	}
	else {
		OTAssert(0, "No input set");
	}
}

QVariant ot::OnePropertyDialog::getCurrentValue(void) const {
	if (m_input) {
		return m_input->getCurrentValue();
	}
	else {
		OTAssert(0, "No input set");
		return QVariant();
	}
}

void ot::OnePropertyDialog::slotValueChanged(void) {
	m_changed = true;
}

void ot::OnePropertyDialog::slotConfirm(void) {
	if ((this->dialogFlags() & DialogCfg::CancelOnNoChange) && !m_changed) {
		this->closeDialog(Dialog::Cancel);
	}
	else {
		this->closeDialog(Dialog::Ok);
	}
}
