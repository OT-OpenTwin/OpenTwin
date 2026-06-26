// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesProjectList.h"

void EntityPropertiesProjectList::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);
	EntityPropertiesProjectList* otherProject = dynamic_cast<EntityPropertiesProjectList*>(other);
	m_value = otherProject->m_value;
	setNeedsUpdate();
}

void EntityPropertiesProjectList::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), m_value, std::list<std::string>());
	newProp->setSpecialType("ProjectList");
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesProjectList::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty)
	{
		OT_LOG_E("Property cast failed");
		return;
	}
	if (m_value != actualProperty->getCurrent())
	{
		setNeedsUpdate();
	}
	m_value = actualProperty->getCurrent();
}

void EntityPropertiesProjectList::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);
	_jsonObject.AddMember("Value", ot::JsonString(m_value, _allocator), _allocator);
}

void EntityPropertiesProjectList::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);
	m_value = ot::json::getString(_object, "Value");
}

EntityPropertiesProjectList* EntityPropertiesProjectList::createProperty(const std::string& group, const std::string& name, const std::string& defaultValue, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Finally create the new property
	EntityPropertiesProjectList* newProperty = new EntityPropertiesProjectList(name);
	newProperty->setValue(TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, defaultValue));
	properties.createProperty(newProperty, group);
	return newProperty;
}
