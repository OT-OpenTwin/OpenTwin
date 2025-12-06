// @otlicense
// File: PropertyInputString.cpp
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
#include "OTGui/PropertyString.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PlainTextEdit.h"
#include "OTWidgets/PropertyInputString.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputString> propertyInputStringRegistrar(ot::PropertyString::propertyTypeString());

ot::PropertyInputString::PropertyInputString(QWidget* _parent) 
	: m_textEdit(nullptr)
{
	m_lineEdit = new LineEdit(_parent);
	m_lineEdit->setFocusPolicy(Qt::ClickFocus);
	this->connect(m_lineEdit, &QLineEdit::editingFinished, this, &PropertyInputString::lclValueChanged);
}

ot::PropertyInputString::~PropertyInputString() {
	if (m_lineEdit) {
		delete m_lineEdit;
		m_lineEdit = nullptr;
	}
	if (m_textEdit) {
		delete m_textEdit;
		m_textEdit = nullptr;
	}
}

void ot::PropertyInputString::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_text.toStdString(), _allocator), _allocator);
}

QVariant ot::PropertyInputString::getCurrentValue(void) const {
	return QVariant(m_text);
}

QWidget* ot::PropertyInputString::getQWidget(void) {
	if (m_lineEdit) {
		return m_lineEdit;
	}
	else if (m_textEdit) {
		return m_textEdit;
	}
	else {
		OT_LOG_E("No widget created");
		return nullptr;
	}
}

const QWidget* ot::PropertyInputString::getQWidget(void) const {
	if (m_lineEdit) {
		return m_lineEdit;
	}
	else if (m_textEdit) {
		return m_textEdit;
	}
	else {
		OT_LOG_E("No widget created");
		return nullptr;
	}
}

void ot::PropertyInputString::lclValueChanged(void) {
	QString txt;
	if (m_lineEdit) {
		txt = m_lineEdit->text();
	}
	else if (m_textEdit) {
		txt = m_textEdit->toPlainText();
	}

	// Avoid input on multiple values
	if (txt == "..." && (this->data().getPropertyFlags() & PropertyBase::HasMultipleValues)) {
		return;
	}
	else if (txt != m_text) {
		m_text = txt;
		this->slotValueChanged();
	}
}

ot::Property* ot::PropertyInputString::createPropertyConfiguration(void) const {
	ot::PropertyString* newProperty = new ot::PropertyString(this->data());

	if (m_lineEdit) {
		newProperty->setMaxLength(m_lineEdit->maxLength());
		newProperty->setPlaceholderText(m_lineEdit->placeholderText().toStdString());
		newProperty->setValue(m_lineEdit->text().toStdString());
		newProperty->setMultiline(false);
	}
	else if (m_textEdit) {
		newProperty->setMaxLength(m_textEdit->getMaxTextLength());
		newProperty->setPlaceholderText(m_textEdit->placeholderText().toStdString());
		newProperty->setValue(m_textEdit->toPlainText().toStdString());
		newProperty->setMultiline(true);
	}
	else {
		OT_LOG_E("No widget created");
	}
	
	return newProperty;
}

bool ot::PropertyInputString::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) {
		return false;
	}

	const PropertyString* actualProperty = dynamic_cast<const PropertyString*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	if (actualProperty->isMultiline()) {
		if (m_lineEdit) {
			delete m_lineEdit;
			m_lineEdit = nullptr;
		}
		if (!m_textEdit) {
			m_textEdit = new PlainTextEdit(nullptr);
			this->connect(m_textEdit, &PlainTextEdit::textChanged, this, &PropertyInputString::lclValueChanged);
		}

		QSignalBlocker sigBlock(m_textEdit);

		m_text = QString::fromStdString(actualProperty->getValue());

		m_textEdit->setPlaceholderText(QString::fromStdString(actualProperty->getPlaceholderText()));

		if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
			m_textEdit->setPlainText("...");
		}
		else {
			m_textEdit->setPlainText(m_text);
		}

		m_textEdit->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
		m_textEdit->setReadOnly(this->data().getPropertyFlags() & Property::IsReadOnly);
		m_textEdit->setMaxTextLength(actualProperty->getMaxLength());
	}
	else {
		if (m_textEdit) {
			delete m_lineEdit;
			m_lineEdit = nullptr;
		}
		if (!m_lineEdit) {
			m_lineEdit = new LineEdit(nullptr);
			this->connect(m_lineEdit, &LineEdit::textChanged, this, &PropertyInputString::lclValueChanged);
		}

		QSignalBlocker sigBlock(m_lineEdit);

		m_text = QString::fromStdString(actualProperty->getValue());

		m_lineEdit->setPlaceholderText(QString::fromStdString(actualProperty->getPlaceholderText()));

		if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
			m_lineEdit->setText("...");
		}
		else {
			m_lineEdit->setText(m_text);
		}

		m_lineEdit->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
		m_lineEdit->setReadOnly(this->data().getPropertyFlags() & Property::IsReadOnly);
		m_lineEdit->setMaxLength(actualProperty->getMaxLength());
	}

	return true;
}

void ot::PropertyInputString::focusPropertyInput(void) {
	if (m_lineEdit) {
		m_lineEdit->setFocus();
	}
	else if (m_textEdit) {
		m_textEdit->setFocus();
	}
}

void ot::PropertyInputString::setText(const QString& _text) {
	m_text = _text;
	if (m_lineEdit) {
		m_lineEdit->setText(m_text);
	}
	else if (m_textEdit) {
		m_textEdit->setPlainText(m_text);
	}
}