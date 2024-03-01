//! @file PropertyInputDouble.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyDouble.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/PropertyInputDouble.h"

ot::PropertyInputDouble::PropertyInputDouble(const PropertyDouble* _property)
	: PropertyInput(_property)
{
	m_spinBox = new DoubleSpinBox;
	m_spinBox->setDecimals(_property->precision());
	m_spinBox->setToolTip(QString::fromStdString(_property->propertyTip()));
	if (_property->propertyFlags() & Property::HasMultipleValues) {
		m_spinBox->setSpecialValueText("??");
		m_spinBox->setValue(_property->min());
	}
	else {
		m_spinBox->setValue(_property->value());
	}
	this->connect(m_spinBox, &QDoubleSpinBox::valueChanged, this, &PropertyInputDouble::lclValueChanged);
}

ot::PropertyInputDouble::~PropertyInputDouble() {
	delete m_spinBox;
}

void ot::PropertyInputDouble::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonValue(m_spinBox->value()), _allocator);
}

QVariant ot::PropertyInputDouble::getCurrentValue(void) const {
	return QVariant(m_spinBox->value());
}

QWidget* ot::PropertyInputDouble::getQWidget(void) {
	return m_spinBox;
}

void ot::PropertyInputDouble::lclValueChanged(int) {
	m_spinBox->setSpecialValueText("");
	PropertyInput::slotValueChanged();
}