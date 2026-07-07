// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyBool.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesBoolean.h"

EntityPropertiesBoolean* EntityPropertiesBoolean::createProperty(const std::string& group, const std::string& name, bool defaultValue, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	bool value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultBool(defaultCategory, name, defaultValue);

	// Finally create the new property
	EntityPropertiesBoolean* newProperty = new EntityPropertiesBoolean(name, value);
	properties.createProperty(newProperty, group);
	return newProperty;
}

void EntityPropertiesBoolean::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	ot::PropertyBool* newProp = new ot::PropertyBool(this->getName(), m_value);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesBoolean::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyBool* actualProperty = dynamic_cast<const ot::PropertyBool*>(_property);
	if (!actualProperty)
	{
		OT_LOG_E("Property cast failed");
		return;
	}

	setValue(actualProperty->getValue());
}

void EntityPropertiesBoolean::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);
	_jsonObject.AddMember("Value", m_value, _allocator);
}

void EntityPropertiesBoolean::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);
	this->setValue(ot::json::getBool(_object, "Value"));
}

void EntityPropertiesBoolean::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesBoolean* entity = dynamic_cast<EntityPropertiesBoolean*>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue());
	}
}

bool EntityPropertiesBoolean::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesBoolean* entity = dynamic_cast<EntityPropertiesBoolean*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}
