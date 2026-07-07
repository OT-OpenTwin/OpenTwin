// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyDouble.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesDouble.h"

EntityPropertiesDouble::EntityPropertiesDouble()
	: m_value(0.), m_allowCustomValues(true), m_min(std::numeric_limits<double>::lowest()), m_max(std::numeric_limits<double>::max()), m_precision(2)
{

}

EntityPropertiesDouble::EntityPropertiesDouble(const std::string& _name, double _value)
	: m_value(_value), m_allowCustomValues(true), m_min(std::numeric_limits<double>::lowest()), m_max(std::numeric_limits<double>::max()), m_precision(2)
{
	this->setName(_name);
}

EntityPropertiesDouble::EntityPropertiesDouble(const EntityPropertiesDouble& _other)
	: EntityPropertiesBase(_other)
{
	m_value = _other.m_value;
	m_allowCustomValues = _other.m_allowCustomValues;
	m_min = _other.m_min;
	m_max = _other.m_max;
	m_suffix = _other.m_suffix;
	m_precision = _other.m_precision;
}

EntityPropertiesDouble& EntityPropertiesDouble::operator=(const EntityPropertiesDouble& _other)
{
	if (&_other != this)
	{
		EntityPropertiesBase::operator=(_other);
		m_value = _other.m_value;
		m_allowCustomValues = _other.m_allowCustomValues;
		m_min = _other.m_min;
		m_max = _other.m_max;
		m_suffix = _other.m_suffix;
		m_precision = _other.m_precision;
	}
	return *this;
}

void EntityPropertiesDouble::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	ot::PropertyDouble* newProp = new ot::PropertyDouble(this->getName(), m_value);
	newProp->setPropertyFlag((m_allowCustomValues ? ot::Property::AllowCustomValues : ot::Property::NoFlags));
	newProp->setRange(m_min, m_max);
	newProp->setSuffix(m_suffix);
	newProp->setPrecision(m_precision);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesDouble::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble*>(_property);
	if (!actualProperty)
	{
		OT_LOG_E("Property cast failed");
		return;
	}

	this->setValue(actualProperty->getValue());
	this->setRange(actualProperty->getMin(), actualProperty->getMax());
	this->setSuffix(actualProperty->getSuffix());
	this->setDecimalPlaces(actualProperty->getPrecision());
	this->setAllowCustomValues(actualProperty->getPropertyFlags() & ot::PropertyBase::AllowCustomValues);
}

void EntityPropertiesDouble::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);

	_jsonObject.AddMember("Value", m_value, _allocator);
	_jsonObject.AddMember("AllowCustom", m_allowCustomValues, _allocator);
	_jsonObject.AddMember("MinValue", m_min, _allocator);
	_jsonObject.AddMember("MaxValue", m_max, _allocator);
	_jsonObject.AddMember("Suffix", ot::JsonString(m_suffix, _allocator), _allocator);
	_jsonObject.AddMember("Decimals", m_precision, _allocator);
}

void EntityPropertiesDouble::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);
	this->setValue(ot::json::getDouble(_object, "Value"));

	if (_object.HasMember("MinValue"))
	{
		this->setMin(ot::json::getDouble(_object, "MinValue", std::numeric_limits<double>::lowest()));
	}
	if (_object.HasMember("MaxValue"))
	{
		this->setMax(ot::json::getDouble(_object, "MaxValue", std::numeric_limits<double>::max()));
	}
	if (_object.HasMember("AllowCustom"))
	{
		this->setAllowCustomValues(ot::json::getBool(_object, "AllowCustom", true));
	}
	if (_object.HasMember("Suffix"))
	{
		this->setSuffix(ot::json::getString(_object, "Suffix"));
	}
	if (_object.HasMember("Decimals"))
	{
		this->setDecimalPlaces(ot::json::getInt(_object, "Decimals"));
	}
}

void EntityPropertiesDouble::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesDouble* entity = dynamic_cast<EntityPropertiesDouble*>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		this->setValue(entity->getValue());
		this->setRange(entity->getMin(), entity->getMax());
		this->setAllowCustomValues(entity->getAllowCustomValues());
		this->setSuffix(entity->getSuffix());
		this->setDecimalPlaces(entity->getDecimalPlaces());
	}
}

void EntityPropertiesDouble::setValue(double _value)
{
	if (m_value != _value)
	{
		this->setNeedsUpdate();
		m_value = _value;
	}
}

void EntityPropertiesDouble::setRange(double _min, double _max)
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

void EntityPropertiesDouble::setMin(double _min)
{
	if (m_min != _min)
	{
		this->setNeedsUpdate();
		m_min = _min;
	}
}

void EntityPropertiesDouble::setMax(double _max)
{
	if (m_max != _max)
	{
		this->setNeedsUpdate();
		m_max = _max;
	}
}

void EntityPropertiesDouble::setAllowCustomValues(bool _allowCustomValues)
{
	if (m_allowCustomValues != _allowCustomValues)
	{
		this->setNeedsUpdate();
		m_allowCustomValues = _allowCustomValues;
	}
}

void EntityPropertiesDouble::setDecimalPlaces(int _precision)
{
	if (m_precision != _precision)
	{
		this->setNeedsUpdate();
		m_precision = _precision;
	}
}

void EntityPropertiesDouble::setSuffix(const std::string& _suffix)
{
	if (m_suffix != _suffix)
	{
		this->setNeedsUpdate();
		m_suffix = _suffix;
	}
}

bool EntityPropertiesDouble::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesDouble* entity = dynamic_cast<EntityPropertiesDouble*>(other);

	if (entity == nullptr)
	{
		return false;
	}

	return (getValue() == entity->getValue());
}

EntityPropertiesDouble* EntityPropertiesDouble::createProperty(const std::string& _group, const std::string& _name, double _defaultValue, const std::string& _defaultCategory, EntityProperties& _properties)
{
	return EntityPropertiesDouble::createProperty(_group, _name, _defaultValue, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), _defaultCategory, _properties);
}

EntityPropertiesDouble* EntityPropertiesDouble::createProperty(const std::string& _group, const std::string& _name, double _defaultValue, double _minValue, double _maxValue, const std::string& _defaultCategory, EntityProperties& _properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(_defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	double value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultDouble(_defaultCategory, _name, _defaultValue);

	// Finally create the new property
	EntityPropertiesDouble* newProperty = new EntityPropertiesDouble(_name, value);
	newProperty->setMin(_minValue);
	newProperty->setMax(_maxValue);
	_properties.createProperty(newProperty, _group);
	return newProperty;
}
