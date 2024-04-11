//! @file PropertyInputInt.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/PropertyInt.h"
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/PropertyInputInt.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputInt> propertyInputIntRegistrar(OT_PROPERTY_TYPE_Int);

ot::PropertyInputInt::PropertyInputInt()
{
	m_spinBox = new SpinBox;
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

void ot::PropertyInputInt::setValue(int _value) {
	m_spinBox->setValue(_value);
}

int ot::PropertyInputInt::getValue(void) const {
	return m_spinBox->value();
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

bool ot::PropertyInputInt::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyInt* actualProperty = dynamic_cast<const PropertyInt*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	m_spinBox->blockSignals(true);

	m_spinBox->setRange(actualProperty->min(), actualProperty->max());
	m_spinBox->setToolTip(this->propertyTip());
	m_spinBox->setValue(actualProperty->value());
	if (this->propertyFlags() & Property::HasMultipleValues) {
		m_spinBox->setSpecialValueText("??");
	}

	m_spinBox->blockSignals(false);

	return true;
}