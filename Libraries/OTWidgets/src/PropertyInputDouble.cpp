//! @file PropertyInputDouble.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/PropertyDouble.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/PropertyInputDouble.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputDouble> propertyInputDoubleRegistrar(OT_PROPERTY_TYPE_Double);

ot::PropertyInputDouble::PropertyInputDouble()
{
	m_spinBox = new DoubleSpinBox;
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

void ot::PropertyInputDouble::setValue(double _value) {
	m_spinBox->setValue(_value);
}

double ot::PropertyInputDouble::getValue(void) const {
	return m_spinBox->value();
}

void ot::PropertyInputDouble::lclValueChanged(int) {
	m_spinBox->setSpecialValueText("");
	PropertyInput::slotValueChanged();
}

ot::Property* ot::PropertyInputDouble::createPropertyConfiguration(void) const {
	ot::PropertyDouble* newProperty = new ot::PropertyDouble(this->data());

	newProperty->setMin(m_spinBox->minimum());
	newProperty->setMax(m_spinBox->maximum());
	newProperty->setValue(m_spinBox->value());

	return newProperty;
}

bool ot::PropertyInputDouble::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	m_spinBox->blockSignals(true);

	m_spinBox->setDecimals(actualProperty->precision());
	m_spinBox->setToolTip(QString::fromStdString(this->data().propertyTip()));
	m_spinBox->setRange(actualProperty->min(), actualProperty->max());
	m_spinBox->setValue(actualProperty->value());
	if (this->data().propertyFlags() & Property::HasMultipleValues) {
		m_spinBox->setSpecialValueText("??");
	}

	m_spinBox->blockSignals(false);

	return true;
}