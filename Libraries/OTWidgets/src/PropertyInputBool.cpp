//! @file PropertyInputBool.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyBool.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/PropertyInputBool.h"

ot::PropertyInputBool::PropertyInputBool(const PropertyBool* _property)
	: PropertyInput(_property)
{
	m_checkBox = new CheckBox;
	m_checkBox->setChecked(_property->value());
	m_checkBox->setToolTip(QString::fromStdString(_property->propertyTip()));
	if (_property->propertyFlags() & Property::HasMultipleValues) {
		m_checkBox->setCheckState(Qt::PartiallyChecked);
	}
	this->connect(m_checkBox, &QCheckBox::stateChanged, this, qOverload<int>(&PropertyInput::slotValueChanged));
}

ot::PropertyInputBool::~PropertyInputBool() {
	delete m_checkBox;
}

void ot::PropertyInputBool::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonValue(m_checkBox->isChecked()), _allocator);
}

QVariant ot::PropertyInputBool::getCurrentValue(void) const {
	return QVariant(m_checkBox->isChecked());
}

QWidget* ot::PropertyInputBool::getQWidget(void) {
	return m_checkBox;
}