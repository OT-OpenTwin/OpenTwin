//! @file PropertyInputString.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/PropertyString.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PropertyInputString.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputString> propertyInputStringRegistrar(OT_PROPERTY_TYPE_String);

ot::PropertyInputString::PropertyInputString(const QString& _text) 
	: m_text(_text)
{
	m_lineEdit = new LineEdit;
	this->connect(m_lineEdit, &QLineEdit::editingFinished, this, &PropertyInputString::lclValueChanged);
}

ot::PropertyInputString::~PropertyInputString() {
	delete m_lineEdit;
}

void ot::PropertyInputString::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_text.toStdString(), _allocator), _allocator);
}

QVariant ot::PropertyInputString::getCurrentValue(void) const {
	return QVariant(m_text);
}

QWidget* ot::PropertyInputString::getQWidget(void) {
	return m_lineEdit;
}

void ot::PropertyInputString::lclValueChanged(void) {
	if (m_lineEdit->text() != m_text) {
		m_text = m_lineEdit->text();
		this->slotValueChanged();
	}
}

ot::Property* ot::PropertyInputString::createPropertyConfiguration(void) const {
	ot::PropertyString* newProperty = new ot::PropertyString;
	newProperty->setPropertyName(this->propertyName());
	newProperty->setPropertyTitle(this->propertyTitle().toStdString());
	newProperty->setPropertyTip(this->propertyTip().toStdString());
	newProperty->setPropertyFlags(this->propertyFlags());

	newProperty->setMaxLength(m_lineEdit->maxLength());
	newProperty->setPlaceholderText(m_lineEdit->placeholderText().toStdString());
	newProperty->setValue(m_lineEdit->text().toStdString());

	return newProperty;
}

bool ot::PropertyInputString::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyString* actualProperty = dynamic_cast<const PropertyString*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	m_lineEdit->blockSignals(true);
	m_text = QString::fromStdString(actualProperty->value());

	m_lineEdit->setPlaceholderText(QString::fromStdString(actualProperty->placeholderText()));

	if (this->propertyFlags() & Property::HasMultipleValues) m_lineEdit->setText("...");
	else m_lineEdit->setText(m_text);

	m_lineEdit->setToolTip(this->propertyTip());

	m_lineEdit->blockSignals(false);

	return true;
}

void ot::PropertyInputString::setText(const QString& _text) {
	m_text = _text;
	m_lineEdit->setText(m_text);
}