//! @file PropertyInputColor.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyColor.h"
#include "OTWidgets/ColorPickButton.h"
#include "OTWidgets/PropertyInputColor.h"

ot::PropertyInputColor::PropertyInputColor(const PropertyColor* _property)
	: PropertyInput(_property)
{
	m_colorBtn = new ColorPickButton(_property->value());
	m_colorBtn->getQWidget()->setContentsMargins(0, 0, 0, 0);
	if (_property->propertyFlags() & Property::HasMultipleValues) {
		m_colorBtn->replaceButtonText("...");
	}
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

ot::Property* ot::PropertyInputColor::createPropertyConfiguration(void) const {
	ot::PropertyColor* newProperty = new ot::PropertyColor;
	newProperty->setPropertyName(this->propertyName());
	newProperty->setPropertyTitle(this->propertyTitle().toStdString());
	newProperty->setPropertyTip(this->propertyTip().toStdString());
	newProperty->setPropertyFlags(this->propertyFlags());
	newProperty->setValue(m_colorBtn->otColor());

	return newProperty;
}