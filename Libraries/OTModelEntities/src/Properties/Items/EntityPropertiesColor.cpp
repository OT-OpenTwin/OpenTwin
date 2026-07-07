// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyColor.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesColor.h"

EntityPropertiesColor* EntityPropertiesColor::createProperty(const std::string& group, const std::string& name, std::vector<int> defaultValue, const std::string& defaultCategory, EntityProperties& properties)
{
	assert(defaultValue.size() == 3);

	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	int valueR = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor(defaultCategory, name, 0, defaultValue[0]);
	int valueG = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor(defaultCategory, name, 1, defaultValue[1]);
	int valueB = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor(defaultCategory, name, 2, defaultValue[2]);

	// Finally create the new property
	EntityPropertiesColor* newProperty = new EntityPropertiesColor(name, valueR / 255.0, valueG / 255.0, valueB / 255.0);
	properties.createProperty(newProperty, group);
	return newProperty;
}

void EntityPropertiesColor::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	ot::Color col((int)(getColorR() * 255.), (int)(getColorG() * 255.), (int)(getColorB() * 255.));
	ot::PropertyColor* newProp = new ot::PropertyColor(this->getName(), col);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesColor::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyColor* actualProperty = dynamic_cast<const ot::PropertyColor*>(_property);
	if (!actualProperty)
	{
		OT_LOG_E("Property cast failed");
		return;
	}
	ot::ColorF c(actualProperty->getValue().toColorF());
	setColorR(c.r());
	setColorG(c.g());
	setColorB(c.b());
}

void EntityPropertiesColor::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);

	_jsonObject.AddMember("ValueR", ot::JsonNumber(getColorR()), _allocator);
	_jsonObject.AddMember("ValueG", ot::JsonNumber(getColorG()), _allocator);
	_jsonObject.AddMember("ValueB", ot::JsonNumber(getColorB()), _allocator);
}

void EntityPropertiesColor::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);
	const rapidjson::Value& valR = _object["ValueR"];
	const rapidjson::Value& valG = _object["ValueG"];
	const rapidjson::Value& valB = _object["ValueB"];

	setColorR(valR.GetDouble());
	setColorG(valG.GetDouble());
	setColorB(valB.GetDouble());
}

EntityPropertiesColor& EntityPropertiesColor::operator=(const EntityPropertiesColor& other)
{
	if (&other != this)
	{
		EntityPropertiesBase::operator=(other);

		setColorR(other.getColorR());
		setColorG(other.getColorG());
		setColorB(other.getColorB());
	}

	return *this;
}

void EntityPropertiesColor::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesColor* entity = dynamic_cast<EntityPropertiesColor*>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setColorRGB(entity->getColorR(), entity->getColorG(), entity->getColorB());
	}
}

bool EntityPropertiesColor::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesColor* entity = dynamic_cast<EntityPropertiesColor*>(other);

	if (entity == nullptr) return false;

	return (getColorR() == entity->getColorR() && getColorG() == entity->getColorG() && getColorB() == entity->getColorB());
}
