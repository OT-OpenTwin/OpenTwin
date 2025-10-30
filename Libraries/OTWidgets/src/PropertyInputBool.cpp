// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyBool.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/PropertyInputBool.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputBool> propertyInputBoolRegistrar(ot::PropertyBool::propertyTypeString());

ot::PropertyInputBool::PropertyInputBool()
{
	m_checkBox = new CheckBox;
	m_checkBox->setFocusPolicy(Qt::NoFocus);
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

const QWidget* ot::PropertyInputBool::getQWidget(void) const {
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

	QSignalBlocker blocker(m_checkBox);

	m_checkBox->setChecked(actualProperty->getValue());
	m_checkBox->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		m_checkBox->setCheckState(Qt::PartiallyChecked);
	}
	m_checkBox->setEnabled(!(this->data().getPropertyFlags() & Property::IsReadOnly));

	return true;
}

void ot::PropertyInputBool::focusPropertyInput(void) {
	m_checkBox->setFocus();
}

void ot::PropertyInputBool::setChecked(bool _isChecked) {
	m_checkBox->setChecked(_isChecked);
}

bool ot::PropertyInputBool::isChecked(void) const {
	return m_checkBox->isChecked();
}