// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesSelection.h"

EntityPropertiesSelection* EntityPropertiesSelection::createProperty(const std::string& group, const std::string& name, const std::list<std::string>& options, const std::string& defaultValue, const std::string& defaultCategory, EntityProperties& properties)
{
	std::list<std::string> optionsCopy = options;
	return EntityPropertiesSelection::createProperty(group, name, std::move(optionsCopy), defaultValue, defaultCategory, properties);
}

EntityPropertiesSelection* EntityPropertiesSelection::createProperty(const std::string& group, const std::string& name, std::list<std::string>&& options, const std::string& defaultValue, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, defaultValue);

	// Finally create the new property
	EntityPropertiesSelection* prop = new EntityPropertiesSelection;
	prop->setName(name);

	for (auto& item : options)
	{
		prop->addOption(item);
	}

	prop->setValue(value);

	properties.createProperty(prop, group);

	return prop;
}

EntityPropertiesSelection::EntityPropertiesSelection() : m_allowCustomValues(false)
{}

EntityPropertiesSelection::EntityPropertiesSelection(const EntityPropertiesSelection& _other)
	: EntityPropertiesBase(_other), m_value(_other.m_value), m_options(_other.m_options), m_allowCustomValues(_other.m_allowCustomValues)
{}

void EntityPropertiesSelection::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), m_value, m_options);
	newProp->setPropertyFlag(ot::Property::AllowCustomValues, m_allowCustomValues);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesSelection::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty)
	{
		OT_LOG_E("Property cast failed");
		return;
	}

	if (m_value != actualProperty->getCurrent()) setNeedsUpdate();
	m_value = actualProperty->getCurrent();

	m_options.clear();
	for (const auto& opt : actualProperty->getOptions())
	{
		m_options.push_back(opt.first);
	}
}

void EntityPropertiesSelection::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);

	_jsonObject.AddMember("Value", ot::JsonString(m_value, _allocator), _allocator);
	_jsonObject.AddMember("Options", ot::JsonArray(m_options, _allocator), _allocator);
	_jsonObject.AddMember("AllowCustom", m_allowCustomValues, _allocator);
}

void EntityPropertiesSelection::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);
	resetOptions(ot::json::getStringList(_object, "Options"));
	if (_object.HasMember("AllowCustom"))
	{
		setAllowCustomValues(ot::json::getBool(_object, "AllowCustom", true));
	}
	setValue(ot::json::getString(_object, "Value"));

}

void EntityPropertiesSelection::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesSelection* entity = dynamic_cast<EntityPropertiesSelection*>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		assert(checkCompatibilityOfSettings(*entity));
		setValue(entity->getValue());
	}
}

void EntityPropertiesSelection::resetOptions(const std::list<std::string>& _options)
{
	resetOptionsImpl(_options);
}

void EntityPropertiesSelection::resetOptions(const std::vector<std::string>& _options)
{
	resetOptionsImpl(_options);
}

bool EntityPropertiesSelection::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesSelection* entity = dynamic_cast<EntityPropertiesSelection*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

bool EntityPropertiesSelection::setValue(const std::string& s)
{
	if (!m_allowCustomValues && std::find(m_options.begin(), m_options.end(), s) == m_options.end())
	{
		return false; // This value is not a valid option
	}

	if (m_value != s)
	{
		setNeedsUpdate();
	}

	m_value = s;
	return true;
}

void EntityPropertiesSelection::setAllowCustomValues(bool _allowCustomValues)
{
	if (m_allowCustomValues != _allowCustomValues)
	{
		this->setNeedsUpdate();
		m_allowCustomValues = _allowCustomValues;
	}
}

bool EntityPropertiesSelection::checkCompatibilityOfSettings(const EntityPropertiesSelection& other) const
{
	if (m_options.size() != other.m_options.size()) return false;

	for (int i = 0; i < m_options.size(); i++)
	{
		if (m_options[i] != other.m_options[i]) return false;
	}

	return true;
}

bool EntityPropertiesSelection::isCompatible(EntityPropertiesBase* other) const
{
	EntityPropertiesSelection* otherItem = dynamic_cast<EntityPropertiesSelection*>(other);
	if (otherItem == nullptr) return false;

	return checkCompatibilityOfSettings(*otherItem);
}
