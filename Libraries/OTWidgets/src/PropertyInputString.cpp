// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyString.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PropertyInputString.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputString> propertyInputStringRegistrar(ot::PropertyString::propertyTypeString());

ot::PropertyInputString::PropertyInputString(const QString& _text) :
	m_text(_text)
{
	m_lineEdit = new LineEdit;
	m_lineEdit->setFocusPolicy(Qt::ClickFocus);
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

const QWidget* ot::PropertyInputString::getQWidget(void) const {
	return m_lineEdit;
}

void ot::PropertyInputString::lclValueChanged(void) {
	// Avoid input on multiple values
	if (m_lineEdit->text() == "..." && (this->data().getPropertyFlags() & PropertyBase::HasMultipleValues)) return;

	if (m_lineEdit->text() != m_text) {
		m_text = m_lineEdit->text();
		this->slotValueChanged();
	}
}

ot::Property* ot::PropertyInputString::createPropertyConfiguration(void) const {
	ot::PropertyString* newProperty = new ot::PropertyString(this->data());

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
	m_text = QString::fromStdString(actualProperty->getValue());

	m_lineEdit->setPlaceholderText(QString::fromStdString(actualProperty->getPlaceholderText()));

	if (this->data().getPropertyFlags() & Property::HasMultipleValues) m_lineEdit->setText("...");
	else m_lineEdit->setText(m_text);

	m_lineEdit->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
	m_lineEdit->setReadOnly(this->data().getPropertyFlags() & Property::IsReadOnly);

	m_lineEdit->blockSignals(false);

	return true;
}

void ot::PropertyInputString::focusPropertyInput(void) {
	m_lineEdit->setFocus();
}

void ot::PropertyInputString::setText(const QString& _text) {
	m_text = _text;
	m_lineEdit->setText(m_text);
}