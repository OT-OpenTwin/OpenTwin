//! @file PropertyInputString.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyString.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PropertyInputString.h"

ot::PropertyInputString::PropertyInputString(const PropertyString* _property)
	: PropertyInput(_property)
{
	m_lineEdit = new LineEdit;
	m_lineEdit->setToolTip(QString::fromStdString(_property->propertyTip()));
	m_lineEdit->setPlaceholderText(QString::fromStdString(_property->placeholderText()));
	m_text = QString::fromStdString(_property->value());
	if (_property->propertyFlags() & Property::HasMultipleValues) {
		m_lineEdit->setText("...");
	}
	else {
		m_lineEdit->setText(QString::fromStdString(_property->value()));
	}
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