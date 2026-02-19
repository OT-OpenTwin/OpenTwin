// @otlicense
// File: PropertyInputStringList.cpp
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
#include "OTCore/Logging/LogDispatcher.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/ComboButton.h"
#include "OTWidgets/PropertyInputStringList.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

// Qt header
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qabstractitemview.h>

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputStringList> propertyInputStringListRegistrar(ot::PropertyStringList::propertyTypeString());

ot::PropertyInputStringList::PropertyInputStringList(QWidget* _parent) 
	: m_delayTimer(this), m_parentWidget(_parent), m_comboBox(nullptr), m_comboButton(nullptr)
{
	m_comboButton = new ComboButton(m_parentWidget);
	connect(m_comboButton, &ComboButton::textChanged, this, &PropertyInputStringList::slotTextInputChanged);

	m_delayTimer.setSingleShot(true);
	m_delayTimer.setInterval(500);
	connect(&m_delayTimer, &QTimer::timeout, this, &PropertyInputStringList::slotTextInputChanged);
}

ot::PropertyInputStringList::~PropertyInputStringList() {
	if (m_comboBox) {
		delete m_comboBox;
		m_comboBox = nullptr;
	}
	if (m_comboButton) {
		delete m_comboButton;
		m_comboButton = nullptr;
	}
}

void ot::PropertyInputStringList::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	if (m_comboBox) {
		_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_comboBox->currentText().toStdString(), _allocator), _allocator);
	}
	else {
		OTAssertNullptr(m_comboButton);
		_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_comboButton->text().toStdString(), _allocator), _allocator);
	}
}

QVariant ot::PropertyInputStringList::getCurrentValue(void) const {
	if (m_comboBox) {
		return QVariant(m_comboBox->currentText());
	}
	else {
		OTAssertNullptr(m_comboButton);
		return QVariant(m_comboButton->text());
	}
}

QWidget* ot::PropertyInputStringList::getQWidget(void) {
	if (m_comboBox) {
		return m_comboBox;
	}
	else {
		OTAssertNullptr(m_comboButton);
		return m_comboButton;
	}
}

const QWidget* ot::PropertyInputStringList::getQWidget(void) const {
	if (m_comboBox) {
		return m_comboBox;
	}
	else {
		OTAssertNullptr(m_comboButton);
		return m_comboButton;
	}
}

ot::Property* ot::PropertyInputStringList::createPropertyConfiguration(void) const {
	ot::PropertyStringList* newProperty = new ot::PropertyStringList(this->data());

	std::list<std::string> lst;
	if (m_comboBox) {
		newProperty->setPropertyFlag(Property::AllowCustomValues, true);
		for (int i = 0; i < m_comboBox->count(); i++) {
			lst.push_back(m_comboBox->itemText(i).toStdString());
		}
		newProperty->setCurrent(m_comboBox->currentText().toStdString());
	}
	else {
		newProperty->setPropertyFlag(Property::AllowCustomValues, false);
		for (QAction* action : m_comboButton->menu()->actions()) {
			lst.push_back(action->text().toStdString());
		}
		newProperty->setCurrent(m_comboButton->text().toStdString());
	}
	newProperty->setList(lst);

	return newProperty;
}

bool ot::PropertyInputStringList::setupFromConfiguration(const Property* _configuration) {
	m_delayTimer.stop();

	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyStringList* actualProperty = dynamic_cast<const PropertyStringList*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	m_currentText = QString::fromStdString(actualProperty->getCurrent());

	QStringList lst;
	for (const std::string& itm : actualProperty->getList()) {
		lst.push_back(QString::fromStdString(itm));
	}
	
	if (data().getPropertyFlags().has(Property::AllowCustomValues)) {
		// Custom values -> Combo Box

		if (m_comboButton) {
			delete m_comboButton;
			m_comboButton = nullptr;
		}
		if (!m_comboBox) {
			m_comboBox = new ComboBox(m_parentWidget);
			m_comboBox->setEditable(true);

			LineEdit* lineEdit = new LineEdit(m_comboBox);
			m_comboBox->setLineEdit(lineEdit);
			connect(m_comboBox, &ComboBox::currentIndexChanged, this, &PropertyInputStringList::slotTextInputChanged);
			connect(lineEdit, &LineEdit::textChanged, [=]() { m_delayTimer.stop(); m_delayTimer.start(); });
		}

		QSignalBlocker blocker(m_comboBox);
		QSignalBlocker blocker2(m_comboBox->lineEdit());

		m_comboBox->clear();
		m_comboBox->addItems(lst);
		m_comboBox->setCurrentText((this->data().getPropertyFlags() & Property::HasMultipleValues) ? "..." : m_currentText);
		m_comboBox->setEnabled(!(this->data().getPropertyFlags() & Property::IsReadOnly));
	}
	else {
		// No custom values -> Combo Button

		if (m_comboBox) {
			delete m_comboBox;
			m_comboBox = nullptr;
		}
		if (!m_comboButton) {
			m_comboButton = new ComboButton(m_parentWidget);
			connect(m_comboButton, &ComboButton::textChanged, this, &PropertyInputStringList::slotTextInputChanged);
		}

		QSignalBlocker blocker(m_comboButton);
		
		m_comboButton->setItems(lst);
		m_comboButton->setText((this->data().getPropertyFlags() & Property::HasMultipleValues) ? "..." : m_currentText);
		m_comboButton->setEnabled(!(this->data().getPropertyFlags() & Property::IsReadOnly));
	}

	return true;
}

void ot::PropertyInputStringList::focusPropertyInput(void) {
	if (m_comboBox) {
		m_comboBox->setFocus();
	}
	else {
		OTAssertNullptr(m_comboButton);
		m_comboButton->setFocus();
	}
}

void ot::PropertyInputStringList::setCurrentText(const QString& _text) {
	if (m_comboBox) {
		m_comboBox->setCurrentText(_text);
	}
	else {
		OTAssertNullptr(m_comboButton);
		m_comboButton->setText(_text);
	}
}

QString ot::PropertyInputStringList::getCurrentText(void) const {
	if (m_comboBox) {
		return m_comboBox->currentText();
	}
	else {
		OTAssertNullptr(m_comboButton);
		return m_comboButton->text();
	}
}

QStringList ot::PropertyInputStringList::getPossibleSelection(void) const {
	QStringList lst;
	if (m_comboBox) {
		for (int i = 0; i < m_comboBox->count(); i++) {
			lst.push_back(m_comboBox->itemText(i));
		}
	}
	else {
		OTAssertNullptr(m_comboButton);
		for (QAction* action : m_comboButton->menu()->actions()) {
			lst.push_back(action->text());
		}
	}
	return lst;
}

void ot::PropertyInputStringList::slotTextInputChanged() {
	QString txt;
	if (m_comboButton) {
		txt = m_comboButton->text();
	}
	else if (m_comboBox) {
		txt = m_comboBox->currentText();
	}

	// Avoid input on multiple values
	if (txt == "..." && (this->data().getPropertyFlags() & PropertyBase::HasMultipleValues)) {
		return;
	}
	else if (txt != m_currentText) {
		m_currentText = txt;
		this->slotValueChanged();
	}
}