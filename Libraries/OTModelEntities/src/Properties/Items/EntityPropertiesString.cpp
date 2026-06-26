// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyString.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesString.h"

EntityPropertiesString* EntityPropertiesString::createProperty(const std::string& group, const std::string& name, const std::string& defaultValue, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, defaultValue);

	// Finally create the new property
	EntityPropertiesString* newProperty = new EntityPropertiesString(name, value);
	newProperty->setIsMultiline(TemplateDefaultManager::getTemplateDefaultManager()->getDefaultBool(defaultCategory, name + "_Multiline", false));
	newProperty->setPlaceholderText(TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name + "_Placeholder", ""));
	properties.createProperty(newProperty, group);
	return newProperty;
}

void EntityPropertiesString::setValue(const std::string& _value)
{
	if (m_value != _value)
	{
		setNeedsUpdate();
		m_value = _value;
	}
}

EntityPropertiesString::EntityPropertiesString()
	: m_isMultiline(false)
{}

EntityPropertiesString::EntityPropertiesString(const std::string& n, const std::string& v)
	: m_value(v), m_isMultiline(false)
{
	this->setName(n);
}

EntityPropertiesString::EntityPropertiesString(const EntityPropertiesString& other)
	: EntityPropertiesBase(other)
{
	m_value = other.m_value;
	m_isMultiline = other.m_isMultiline;
	m_placeholderText = other.m_placeholderText;
}

void EntityPropertiesString::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	ot::PropertyString* newProp = new ot::PropertyString(this->getName(), m_value);
	this->setupPropertyData(_configuration, newProp);
	newProp->setMultiline(m_isMultiline);
	newProp->setPlaceholderText(m_placeholderText);
}

void EntityPropertiesString::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyString* actualProperty = dynamic_cast<const ot::PropertyString*>(_property);
	if (!actualProperty)
	{
		OT_LOG_E("Property cast failed");
		return;
	}

	setValue(actualProperty->getValue());
	setIsMultiline(actualProperty->isMultiline());
	setPlaceholderText(actualProperty->getPlaceholderText());
}

void EntityPropertiesString::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);
	_jsonObject.AddMember("Value", ot::JsonString(m_value, _allocator), _allocator);
	_jsonObject.AddMember("IsMultiline", m_isMultiline, _allocator);
	_jsonObject.AddMember("PlaceholderText", ot::JsonString(m_placeholderText, _allocator), _allocator);
}

void EntityPropertiesString::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);
	this->setValue(ot::json::getString(_object, "Value"));
	if (_object.HasMember("IsMultiline"))
	{
		this->setIsMultiline(ot::json::getBool(_object, "IsMultiline", false));
	}
	if (_object.HasMember("PlaceholderText"))
	{
		this->setPlaceholderText(ot::json::getString(_object, "PlaceholderText"));
	}

}

void EntityPropertiesString::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesString* entity = dynamic_cast<EntityPropertiesString*>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue());
		setIsMultiline(entity->getIsMultiline());
		setPlaceholderText(entity->getPlaceholderText());
	}
}

bool EntityPropertiesString::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesString* entity = dynamic_cast<EntityPropertiesString*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}
