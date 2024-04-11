//! @file OnePropertyDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/Property.h"
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
	QLabel* l = new QLabel(QString::fromStdString(_config.getProperty()->propertyTitle()));
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
	this->connect(btnCancel, &QPushButton::clicked, this, &OnePropertyDialog::slotCancel);
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
		this->close(Dialog::Cancel);
	}
	else {
		this->close(Dialog::Ok);
	}
}

void ot::OnePropertyDialog::slotCancel(void) {
	this->close(Dialog::Cancel);
}
