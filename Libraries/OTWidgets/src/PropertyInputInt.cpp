//! @file PropertyInputInt.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyInt.h"
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/PropertyInputInt.h"

ot::PropertyInputInt::PropertyInputInt(const PropertyInt* _property) {
	m_spinBox = new SpinBox;
	m_spinBox->setRange(_property->min(), _property->max());
	m_spinBox->setValue(_property->value());
	this->connect(m_spinBox, &QSpinBox::valueChanged, this, qOverload<int>(&PropertyInput::slotValueChanged));
}

ot::PropertyInputInt::~PropertyInputInt() {
	delete m_spinBox;
}

void ot::PropertyInputInt::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberName, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberName, _allocator), JsonValue(m_spinBox->value()), _allocator);
}

QVariant ot::PropertyInputInt::getCurrentValue(void) const {
	return QVariant(m_spinBox->value());
}

QWidget* ot::PropertyInputInt::getQWidget(void) {
	return m_spinBox;
}