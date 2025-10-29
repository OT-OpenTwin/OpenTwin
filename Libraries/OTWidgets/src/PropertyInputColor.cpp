// @otlicense

//! @file PropertyInputColor.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyColor.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ColorPickButton.h"
#include "OTWidgets/PropertyInputColor.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputColor> propertyInputColorRegistrar(ot::PropertyColor::propertyTypeString());

ot::PropertyInputColor::PropertyInputColor()
{
	m_colorBtn = new ColorPickButton;
	m_colorBtn->getQWidget()->setContentsMargins(0, 0, 0, 0);
	m_colorBtn->getPushButton()->setObjectName("OT_PropertyInputColor_Button");
	this->connect(m_colorBtn, &ColorPickButton::colorChanged, this, qOverload<>(&PropertyInput::slotValueChanged));
}

ot::PropertyInputColor::~PropertyInputColor() {
	delete m_colorBtn;
}

void ot::PropertyInputColor::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	JsonObject colorObj;
	ot::Color c(m_colorBtn->color().red(), m_colorBtn->color().green(), m_colorBtn->color().blue(), m_colorBtn->color().alpha());
	c.addToJsonObject(colorObj, _allocator);
	_object.AddMember(JsonString(_memberNameValue, _allocator), colorObj, _allocator);
}

QVariant ot::PropertyInputColor::getCurrentValue(void) const {
	return QVariant(m_colorBtn->color());
}

QWidget* ot::PropertyInputColor::getQWidget(void) {
	return m_colorBtn->getQWidget();
}

const QWidget* ot::PropertyInputColor::getQWidget(void) const {
	return m_colorBtn->getQWidget();
}

ot::Property* ot::PropertyInputColor::createPropertyConfiguration(void) const {
	ot::PropertyColor* newProperty = new ot::PropertyColor(this->data());

	newProperty->setValue(m_colorBtn->otColor());

	return newProperty;
}

bool ot::PropertyInputColor::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyColor* actualProperty = dynamic_cast<const PropertyColor*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	m_colorBtn->blockSignals(true);

	m_colorBtn->setColor(actualProperty->getValue());
	m_colorBtn->setEditAlpha(actualProperty->getIncludeAlpha());

	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		m_colorBtn->replaceButtonText("...");
	}
	m_colorBtn->getPushButton()->setEnabled(!(this->data().getPropertyFlags() & Property::IsReadOnly));

	m_colorBtn->blockSignals(false);

	return true;
}

void ot::PropertyInputColor::focusPropertyInput(void) {
	m_colorBtn->setFocus();
}

void ot::PropertyInputColor::setColor(const Color& _color) {
	m_colorBtn->setColor(_color);
}

void ot::PropertyInputColor::setColor(const QColor& _color) {
	m_colorBtn->setColor(_color);
}

ot::Color ot::PropertyInputColor::getOTColor(void) const {
	return m_colorBtn->otColor();
}

QColor ot::PropertyInputColor::getColor(void) const {
	return m_colorBtn->color();
}