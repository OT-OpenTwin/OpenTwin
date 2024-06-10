//! @file PropertyInputBool.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyBool.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/PropertyInputBool.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputBool> propertyInputBoolRegistrar(OT_PROPERTY_TYPE_Bool);

ot::PropertyInputBool::PropertyInputBool()
{
	m_checkBox = new CheckBox;
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

ot::Property* ot::PropertyInputBool::createPropertyConfiguration(void) const {
	ot::PropertyBool* newProperty = new ot::PropertyBool(this->data());
	newProperty->setValue(m_checkBox->isChecked());

	return newProperty;
}

bool ot::PropertyInputBool::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	m_checkBox->blockSignals(true);

	m_checkBox->setChecked(actualProperty->value());
	m_checkBox->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		m_checkBox->setCheckState(Qt::PartiallyChecked);
	}
	m_checkBox->setEnabled(!(this->data().getPropertyFlags() & Property::IsReadOnly));

	m_checkBox->blockSignals(false);

	return true;
}

void ot::PropertyInputBool::setChecked(bool _isChecked) {
	m_checkBox->setChecked(_isChecked);
}

bool ot::PropertyInputBool::isChecked(void) const {
	return m_checkBox->isChecked();
}