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
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyStringList.h"
#include "OTWidgets/ComboButton.h"
#include "OTWidgets/PropertyInputStringList.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

// Qt header
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qabstractitemview.h>

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputStringList> propertyInputStringListRegistrar(ot::PropertyStringList::propertyTypeString());

ot::PropertyInputStringList::PropertyInputStringList()
{
	m_comboButton = new ComboButton;
	this->connect(m_comboButton, &ComboButton::textChanged, this, qOverload<>(&PropertyInput::slotValueChanged));
}

ot::PropertyInputStringList::~PropertyInputStringList() {
	delete m_comboButton;
}

void ot::PropertyInputStringList::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_comboButton->text().toStdString(), _allocator), _allocator);
}

QVariant ot::PropertyInputStringList::getCurrentValue(void) const {
	return QVariant(m_comboButton->text());
}

QWidget* ot::PropertyInputStringList::getQWidget(void) {
	return m_comboButton;
}

const QWidget* ot::PropertyInputStringList::getQWidget(void) const {
	return m_comboButton;
}

ot::Property* ot::PropertyInputStringList::createPropertyConfiguration(void) const {
	ot::PropertyStringList* newProperty = new ot::PropertyStringList(this->data());

	std::list<std::string> lst;
	for (const QAction* a : m_comboButton->menu()->actions()) {
		lst.push_back(a->text().toStdString());
	}
	newProperty->setList(lst);
	newProperty->setCurrent(m_comboButton->text().toStdString());

	return newProperty;
}

bool ot::PropertyInputStringList::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyStringList* actualProperty = dynamic_cast<const PropertyStringList*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	QStringList lst;
	for (const std::string& itm : actualProperty->getList()) {
		lst.push_back(QString::fromStdString(itm));
	}
	
	m_comboButton->blockSignals(true);

	m_comboButton->setItems(lst);
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		m_comboButton->setText("...");
	}
	else {
		m_comboButton->setText(QString::fromStdString(actualProperty->getCurrent()));
	}

	m_comboButton->setEnabled(!(this->data().getPropertyFlags() & Property::IsReadOnly));

	m_comboButton->blockSignals(false);

	return true;
}

void ot::PropertyInputStringList::focusPropertyInput(void) {
	m_comboButton->setFocus();
}

void ot::PropertyInputStringList::setCurrentText(const QString& _text) {
	m_comboButton->setText(_text);
}

QString ot::PropertyInputStringList::getCurrentText(void) const {
	return m_comboButton->text();
}

QStringList ot::PropertyInputStringList::getPossibleSelection(void) const {
	QStringList lst;
	for (const QAction* a : m_comboButton->menu()->actions()) {
		lst.append(a->text());
	}
	return lst;
}
