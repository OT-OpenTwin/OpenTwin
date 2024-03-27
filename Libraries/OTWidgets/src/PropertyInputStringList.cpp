//! @file PropertyInputStringList.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyStringList.h"
#include "OTWidgets/ComboButton.h"
#include "OTWidgets/PropertyInputStringList.h"

// Qt header
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qaction.h>

ot::PropertyInputStringList::PropertyInputStringList(const PropertyStringList* _property)
	: PropertyInput(_property)
{
	QStringList lst;
	for (const std::string& itm : _property->list()) {
		lst.push_back(QString::fromStdString(itm));
	}
	m_comboButton = new ComboButton(QString::fromStdString(_property->current()), lst);
	if (this->propertyFlags() & Property::HasMultipleValues) m_comboButton->setText("...");
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

ot::Property* ot::PropertyInputStringList::createPropertyConfiguration(void) const {
	ot::PropertyStringList* newProperty = new ot::PropertyStringList;
	newProperty->setPropertyName(this->propertyName());
	newProperty->setPropertyTitle(this->propertyTitle().toStdString());
	newProperty->setPropertyTip(this->propertyTip().toStdString());
	newProperty->setPropertyFlags(this->propertyFlags());

	std::list<std::string> lst;
	for (const QAction* a : m_comboButton->menu()->actions()) {
		lst.push_back(a->text().toStdString());
	}
	newProperty->setList(lst);
	newProperty->setCurrent(m_comboButton->text().toStdString());

	return newProperty;
}