//! @file PropertyInputString.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyString.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PropertyInputString.h"

ot::PropertyInputString::PropertyInputString(const QString& _text) 
	: m_text(_text)
{}

ot::PropertyInputString::PropertyInputString(const PropertyString* _property)
	: PropertyInput(_property)
{
	m_text = QString::fromStdString(_property->value());
	this->ini();
	m_lineEdit->setPlaceholderText(QString::fromStdString(_property->placeholderText()));
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

void ot::PropertyInputString::ini() {
	m_lineEdit = new LineEdit;
	if (this->propertyFlags() & Property::HasMultipleValues) m_lineEdit->setText("...");
	else m_lineEdit->setText(m_text);


	m_lineEdit->setToolTip(this->propertyTip());
	this->connect(m_lineEdit, &QLineEdit::editingFinished, this, &PropertyInputString::lclValueChanged);
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

void ot::PropertyInputString::setText(const QString& _text) {
	m_text = _text;
	m_lineEdit->setText(m_text);
}