//! @file PropertyInputInt.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyInt.h"
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/PropertyInputInt.h"

ot::PropertyInputInt::PropertyInputInt(const PropertyInt* _property)
	: PropertyInput(_property) 
{
	m_spinBox = new SpinBox;
	m_spinBox->setRange(_property->min(), _property->max());
	m_spinBox->setToolTip(QString::fromStdString(_property->propertyTip()));
	if (_property->propertyFlags() & Property::HasMultipleValues) {
		m_spinBox->setSpecialValueText("??");
		m_spinBox->setValue(_property->min());
	}
	else {
		m_spinBox->setValue(_property->value());
	}
	this->connect(m_spinBox, &QSpinBox::valueChanged, this, &PropertyInputInt::lclValueChanged);
}

ot::PropertyInputInt::~PropertyInputInt() {
	delete m_spinBox;
}

void ot::PropertyInputInt::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonValue(m_spinBox->value()), _allocator);
}

QVariant ot::PropertyInputInt::getCurrentValue(void) const {
	return QVariant(m_spinBox->value());
}

QWidget* ot::PropertyInputInt::getQWidget(void) {
	return m_spinBox;
}

void ot::PropertyInputInt::lclValueChanged(int) {
	m_spinBox->setSpecialValueText("");
	PropertyInput::slotValueChanged();
}

ot::Property* ot::PropertyInputInt::createPropertyConfiguration(void) const {
	ot::PropertyInt* newProperty = new ot::PropertyInt;
	newProperty->setPropertyName(this->propertyName());
	newProperty->setPropertyTitle(this->propertyTitle().toStdString());
	newProperty->setPropertyTip(this->propertyTip().toStdString());
	newProperty->setPropertyFlags(this->propertyFlags());

	newProperty->setMin(m_spinBox->minimum());
	newProperty->setMax(m_spinBox->maximum());
	newProperty->setValue(m_spinBox->value());

	return newProperty;
}