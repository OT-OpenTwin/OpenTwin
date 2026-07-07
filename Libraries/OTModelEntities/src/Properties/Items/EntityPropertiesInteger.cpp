// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyInt.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesInteger.h"

EntityPropertiesInteger::EntityPropertiesInteger()
	: m_value(0), m_allowCustomValues(true), m_min(std::numeric_limits<long>::lowest()), m_max(std::numeric_limits<long>::max())
{}

EntityPropertiesInteger::EntityPropertiesInteger(const std::string& _name, long _value)
	: m_value(_value), m_allowCustomValues(true), m_min(std::numeric_limits<long>::lowest()), m_max(std::numeric_limits<long>::max())
{
	this->setName(_name);
}

EntityPropertiesInteger::EntityPropertiesInteger(const EntityPropertiesInteger& _other)
	: EntityPropertiesBase(_other)
{
	m_value = _other.getValue();
	m_allowCustomValues = _other.m_allowCustomValues;
	m_min = _other.m_min;
	m_max = _other.m_max;
	m_suffix = _other.m_suffix;
}

EntityPropertiesInteger& EntityPropertiesInteger::operator=(const EntityPropertiesInteger& _other)
{
	if (&_other != this)
	{
		EntityPropertiesBase::operator=(_other);
		m_value = _other.getValue();
		m_allowCustomValues = _other.m_allowCustomValues;
		m_min = _other.m_min;
		m_max = _other.m_max;
		m_suffix = _other.m_suffix;
	}
	return *this;
}

void EntityPropertiesInteger::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	ot::PropertyInt* newProp = new ot::PropertyInt(this->getName(), m_value);
	newProp->setPropertyFlag((m_allowCustomValues ? ot::Property::AllowCustomValues : ot::Property::NoFlags));
	newProp->setRange(m_min, m_max);
	newProp->setSuffix(m_suffix);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesInteger::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyInt* actualProperty = dynamic_cast<const ot::PropertyInt*>(_property);
	if (!actualProperty)
	{
		OT_LOG_E("Property cast failed");
		return;
	}

	this->setValue(actualProperty->getValue());
	this->setRange(actualProperty->getMin(), actualProperty->getMax());
	this->setSuffix(actualProperty->getSuffix());
	this->setAllowCustomValues(actualProperty->getPropertyFlags() & ot::PropertyBase::AllowCustomValues);
}

void EntityPropertiesInteger::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);

	_jsonObject.AddMember("Value", ot::JsonNumber(m_value), _allocator);
	_jsonObject.AddMember("AllowCustom", m_allowCustomValues, _allocator);
	_jsonObject.AddMember("MinValue", ot::JsonNumber(m_min), _allocator);
	_jsonObject.AddMember("MaxValue", ot::JsonNumber(m_max), _allocator);
	_jsonObject.AddMember("Suffix", ot::JsonString(m_suffix, _allocator), _allocator);
}

void EntityPropertiesInteger::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);
	this->setValue((long)ot::json::getInt64(_object, "Value"));
	if (_object.HasMember("MinValue"))
	{
		this->setMin((long)ot::json::getInt64(_object, "MinValue", std::numeric_limits<long>::lowest()));
	}
	if (_object.HasMember("MaxValue"))
	{
		this->setMax((long)ot::json::getInt64(_object, "MaxValue", std::numeric_limits<long>::max()));
	}
	if (_object.HasMember("AllowCustom"))
	{
		this->setAllowCustomValues(ot::json::getBool(_object, "AllowCustom", true));
	}
	if (_object.HasMember("Suffix"))
	{
		this->setSuffix(ot::json::getString(_object, "Suffix"));
	}
}

void EntityPropertiesInteger::setValue(long _value)
{
	if (m_value != _value)
	{
		this->setNeedsUpdate();
		m_value = _value;
	}
}

void EntityPropertiesInteger::setRange(long _min, long _max)
{
	if (m_min != _min)
	{
		this->setNeedsUpdate();
		m_min = _min;
	}
	if (m_max != _max)
	{
		this->setNeedsUpdate();
		m_max = _max;
	}
}

void EntityPropertiesInteger::setMin(long _min)
{
	if (m_min != _min)
	{
		this->setNeedsUpdate();
		m_min = _min;
	}
}

void EntityPropertiesInteger::setMax(long _max)
{
	if (m_max != _max)
	{
		this->setNeedsUpdate();
		m_max = _max;
	}
}

void EntityPropertiesInteger::setAllowCustomValues(bool _allowCustomValues)
{
	if (m_allowCustomValues != _allowCustomValues)
	{
		this->setNeedsUpdate();
		m_allowCustomValues = _allowCustomValues;
	}
}

void EntityPropertiesInteger::setSuffix(const std::string& _suffix)
{
	if (m_suffix != _suffix)
	{
		this->setNeedsUpdate();
		m_suffix = _suffix;
	}
}

bool EntityPropertiesInteger::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesInteger* entity = dynamic_cast<EntityPropertiesInteger*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

void EntityPropertiesInteger::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesInteger* entity = dynamic_cast<EntityPropertiesInteger*>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		this->setValue(entity->getValue());
		this->setRange(entity->getMin(), entity->getMax());
		this->setAllowCustomValues(entity->getAllowCustomValues());
		this->setSuffix(entity->getSuffix());
	}
}

EntityPropertiesInteger* EntityPropertiesInteger::createProperty(const std::string& _group, const std::string& _name, long _defaultValue, const std::string& _defaultCategory, EntityProperties& _properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(_defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	long value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultLong(_defaultCategory, _name, _defaultValue);

	// Finally create the new property
	EntityPropertiesInteger* newProperty = new EntityPropertiesInteger(_name, value);
	_properties.createProperty(newProperty, _group);
	return newProperty;
}

EntityPropertiesInteger* EntityPropertiesInteger::createProperty(const std::string& _group, const std::string& _name, long _defaultValue, long _minValue, long _maxValue, const std::string& _defaultCategory, EntityProperties& _properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(_defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	long value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultLong(_defaultCategory, _name, _defaultValue);

	// Finally create the new property
	EntityPropertiesInteger* newProperty = new EntityPropertiesInteger(_name, value);
	newProperty->setMin(_minValue);
	newProperty->setMax(_maxValue);
	_properties.createProperty(newProperty, _group);
	return newProperty;
}
