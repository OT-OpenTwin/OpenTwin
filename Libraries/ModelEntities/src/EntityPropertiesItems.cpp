#include <cassert>

#include "EntityPropertiesItems.h"
#include "EntityProperties.h"
#include "EntityContainer.h"
#include "TemplateDefaultManager.h"

#include "OTCore/Logger.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyStringList.h"

void EntityPropertiesBase::setNeedsUpdate(void)
{
	if (container != nullptr) container->setNeedsUpdate(); 
	
	needsUpdateFlag = true; 
}

EntityPropertiesBase::EntityPropertiesBase(const EntityPropertiesBase &other)
{ 
	name = other.name; 
	group = other.group;

	multipleValues = other.multipleValues; 
	container = other.container;
	needsUpdateFlag = other.needsUpdateFlag;
	readOnly = other.readOnly;
	protectedProperty = other.protectedProperty;
	visible = other.visible;
	errorState = other.errorState;
}

void EntityPropertiesBase::copySettings(EntityPropertiesBase *other, EntityBase *root)
{ 
	assert(name == other->getName()); 

	// We keep the container unchanged, since we don't want to overwrite the ownership of this item
	// We also dont assign the group, since the group assignment of a property can not be changed
	multipleValues = other->multipleValues;
	needsUpdateFlag = other->needsUpdateFlag;
	readOnly = other->readOnly;
	protectedProperty = other->protectedProperty;
	visible = other->visible;
	errorState = other->errorState;
}

EntityPropertiesBase& EntityPropertiesBase::operator=(const EntityPropertiesBase &other)
{ 
	if (&other != this) 
	{ 
		// We keep the container unchanged, since we don't want to overwrite the ownership of this item
		name = other.name;
		group = other.group;
		multipleValues = other.multipleValues; 
		needsUpdateFlag = other.needsUpdateFlag;
		readOnly = other.readOnly;
		protectedProperty = other.protectedProperty;
		visible = other.visible;
		errorState = other.errorState;
	}
	
	return *this; 
}

void EntityPropertiesBase::setupPropertyData(ot::PropertyGridCfg& _configuration, ot::Property* _property)
{
	if (this->hasMultipleValues()) _property->setPropertyFlag(ot::Property::HasMultipleValues);
	if (this->getReadOnly()) _property->setPropertyFlag(ot::Property::IsReadOnly);
	if (!this->getProtected()) _property->setPropertyFlag(ot::Property::IsDeletable);
	if (!this->getVisible()) _property->setPropertyFlag(ot::Property::IsHidden);
	if (this->getErrorState()) _property->setPropertyFlag(ot::Property::HasInputError);

	ot::PropertyGroup* group = _configuration.findOrCreateGroup(this->getGroup());
	OTAssertNullptr(group);
	group->addProperty(_property);
}

void EntityPropertiesBase::addBaseDataToJsonDocument(ot::JsonValue& container, ot::JsonAllocator& allocator, const std::string& type)
{
	container.AddMember("Type", ot::JsonString(type, allocator), allocator);
	container.AddMember("MultipleValues", this->hasMultipleValues(), allocator);
	container.AddMember("ReadOnly", this->getReadOnly(), allocator);
	container.AddMember("Protected", this->getProtected(), allocator);
	container.AddMember("Visible", this->getVisible(), allocator);
	container.AddMember("ErrorState", this->getErrorState(), allocator);
	container.AddMember("Group", ot::JsonString(this->getGroup(), allocator), allocator);
}

bool EntityPropertiesBase::needsUpdate(void)
{
	return needsUpdateFlag;
}

// ################################################################################################################################################################

EntityPropertiesDouble::EntityPropertiesDouble() 
	: m_value(0.), m_allowCustomValues(true), m_min(std::numeric_limits<double>::lowest()), m_max(std::numeric_limits<double>::max())
{

}

EntityPropertiesDouble::EntityPropertiesDouble(const std::string& _name, double _value) 
	: m_value(_value), m_allowCustomValues(true), m_min(std::numeric_limits<double>::lowest()), m_max(std::numeric_limits<double>::max())
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
}

EntityPropertiesDouble& EntityPropertiesDouble::operator=(const EntityPropertiesDouble& _other) {
	if (&_other != this) {
		EntityPropertiesBase::operator=(_other);
		m_value = _other.m_value;
		m_allowCustomValues = _other.m_allowCustomValues;
		m_min = _other.m_min;
		m_max = _other.m_max;
	}
	return *this;
}

void EntityPropertiesDouble::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyDouble* newProp = new ot::PropertyDouble(this->getName(), m_value);
	newProp->setPropertyFlag((m_allowCustomValues ? ot::Property::AllowCustomValues : ot::Property::NoFlags));
	newProp->setRange(m_min, m_max);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesDouble::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{
	const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble *>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	this->setValue(actualProperty->getValue());
	this->setRange(actualProperty->getMin(), actualProperty->getMax());
	this->setAllowCustomValues(actualProperty->getPropertyFlags() & ot::PropertyBase::AllowCustomValues);
}

void EntityPropertiesDouble::addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root)
{
	ot::JsonObject container;
	EntityPropertiesBase::addBaseDataToJsonDocument(container, jsonDoc.GetAllocator(), "double");

	container.AddMember("Value", m_value, jsonDoc.GetAllocator());
	container.AddMember("AllowCustom", m_allowCustomValues, jsonDoc.GetAllocator());
	container.AddMember("MinValue", m_min, jsonDoc.GetAllocator());
	container.AddMember("MaxValue", m_max, jsonDoc.GetAllocator());

	jsonDoc.AddMember(ot::JsonString(this->getName(), jsonDoc.GetAllocator()), container, jsonDoc.GetAllocator());
}

void EntityPropertiesDouble::readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root)
{
	this->setValue(ot::json::getDouble(object, "Value"));

	if (object.HasMember("MinValue")) {
		this->setMin(ot::json::getDouble(object, "MinValue", std::numeric_limits<double>::lowest()));
	}
	if (object.HasMember("MaxValue")) {
		this->setMax(ot::json::getDouble(object, "MaxValue", std::numeric_limits<double>::max()));
	}
	if (object.HasMember("AllowCustom")) {
		this->setAllowCustomValues(ot::json::getBool(object, "AllowCustom", true));
	}
}

void EntityPropertiesDouble::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesDouble *entity = dynamic_cast<EntityPropertiesDouble *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		this->setValue(entity->getValue());
		this->setRange(entity->getMin(), entity->getMax());
		this->setAllowCustomValues(entity->getAllowCustomValues());
	}
}

void EntityPropertiesDouble::setValue(double _value) {
	if (m_value != _value) {
		this->setNeedsUpdate();
		m_value = _value;
	}
}

void EntityPropertiesDouble::setRange(double _min, double _max) {
	if (m_min != _min) {
		this->setNeedsUpdate();
		m_min = _min;
	}
	if (m_max != _max) {
		this->setNeedsUpdate();
		m_max = _max;
	}
}

void EntityPropertiesDouble::setMin(double _min) {
	if (m_min != _min) {
		this->setNeedsUpdate();
		m_min = _min;
	}
}

void EntityPropertiesDouble::setMax(double _max) {
	if (m_max != _max) {
		this->setNeedsUpdate();
		m_max = _max;
	}
}

void EntityPropertiesDouble::setAllowCustomValues(bool _allowCustomValues) {
	if (m_allowCustomValues != _allowCustomValues) {
		this->setNeedsUpdate();
		m_allowCustomValues = _allowCustomValues;
	}
}

bool EntityPropertiesDouble::hasSameValue(EntityPropertiesBase* other) {
	EntityPropertiesDouble* entity = dynamic_cast<EntityPropertiesDouble*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

void EntityPropertiesDouble::createProperty(const std::string& group, const std::string& name, double defaultValue, const std::string& defaultCategory, EntityProperties& properties) {
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	double value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultDouble(defaultCategory, name, defaultValue);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesDouble(name, value), group);
}

// ################################################################################################################################################################

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
}

EntityPropertiesInteger& EntityPropertiesInteger::operator=(const EntityPropertiesInteger& _other) {
	if (&_other != this) {
		EntityPropertiesBase::operator=(_other);
		m_value = _other.getValue();
		m_allowCustomValues = _other.m_allowCustomValues;
		m_min = _other.m_min;
		m_max = _other.m_max;
	}
	return *this;
}

void EntityPropertiesInteger::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyInt* newProp = new ot::PropertyInt(this->getName(), m_value);
	newProp->setPropertyFlag((m_allowCustomValues ? ot::Property::AllowCustomValues : ot::Property::NoFlags));
	newProp->setRange(m_min, m_max);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesInteger::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{
	const ot::PropertyInt* actualProperty = dynamic_cast<const ot::PropertyInt*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	this->setValue(actualProperty->getValue());
	this->setRange(actualProperty->getMin(), actualProperty->getMax());
	this->setAllowCustomValues(actualProperty->getPropertyFlags() & ot::PropertyBase::AllowCustomValues);
}

void EntityPropertiesInteger::addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root) {
	ot::JsonAllocator& allocator = jsonDoc.GetAllocator();

	ot::JsonObject container;
	EntityPropertiesBase::addBaseDataToJsonDocument(container, allocator, "integer");

	container.AddMember("Value", ot::JsonNumber(m_value), allocator);
	container.AddMember("AllowCustom", m_allowCustomValues, jsonDoc.GetAllocator());
	container.AddMember("MinValue", ot::JsonNumber(m_min), jsonDoc.GetAllocator());
	container.AddMember("MaxValue", ot::JsonNumber(m_max), jsonDoc.GetAllocator());

	jsonDoc.AddMember(ot::JsonString(this->getName(), jsonDoc.GetAllocator()), container, allocator);
}

void EntityPropertiesInteger::readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root)
{
	this->setValue(ot::json::getInt64(object, "Value"));
	if (object.HasMember("MinValue")) {
		this->setMin(ot::json::getInt64(object, "MinValue", std::numeric_limits<long>::lowest()));
	}
	if (object.HasMember("MaxValue")) {
		this->setMax(ot::json::getInt64(object, "MaxValue", std::numeric_limits<long>::max()));
	}
	if (object.HasMember("AllowCustom")) {
		this->setAllowCustomValues(ot::json::getBool(object, "AllowCustom", true));
	}
}

void EntityPropertiesInteger::setValue(long _value) {
	if (m_value != _value) {
		this->setNeedsUpdate();
		m_value = _value;
	}
}

void EntityPropertiesInteger::setRange(long _min, long _max) {
	if (m_min != _min) {
		this->setNeedsUpdate();
		m_min = _min;
	}
	if (m_max != _max) {
		this->setNeedsUpdate();
		m_max = _max;
	}
}

void EntityPropertiesInteger::setMin(long _min) {
	if (m_min != _min) {
		this->setNeedsUpdate();
		m_min = _min;
	}
}

void EntityPropertiesInteger::setMax(long _max) {
	if (m_max != _max) {
		this->setNeedsUpdate();
		m_max = _max;
	}
}

void EntityPropertiesInteger::setAllowCustomValues(bool _allowCustomValues) {
	if (m_allowCustomValues != _allowCustomValues) {
		this->setNeedsUpdate();
		m_allowCustomValues = _allowCustomValues;
	}
}

bool EntityPropertiesInteger::hasSameValue(EntityPropertiesBase* other)
{
	EntityPropertiesInteger* entity = dynamic_cast<EntityPropertiesInteger*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

void EntityPropertiesInteger::copySettings(EntityPropertiesBase* other, EntityBase* root) {
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesInteger* entity = dynamic_cast<EntityPropertiesInteger*>(other);
	assert(entity != nullptr);

	if (entity != nullptr) {
		this->setValue(entity->getValue());
		this->setRange(entity->getMin(), entity->getMax());
		this->setAllowCustomValues(entity->getAllowCustomValues());
	}
}

void EntityPropertiesInteger::createProperty(const std::string& group, const std::string& name, long defaultValue, const std::string& defaultCategory, EntityProperties& properties) {
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	long value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultLong(defaultCategory, name, defaultValue);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesInteger(name, value), group);
}

// ################################################################################################################################################################

void EntityPropertiesBoolean::createProperty(const std::string &group, const std::string &name, bool defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	bool value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultBool(defaultCategory, name, defaultValue);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesBoolean(name, value), group);
}

void EntityPropertiesBoolean::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyBool* newProp = new ot::PropertyBool(this->getName(), value);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesBoolean::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{
	const ot::PropertyBool* actualProperty = dynamic_cast<const ot::PropertyBool*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	setValue(actualProperty->getValue());
}

void EntityPropertiesBoolean::addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root)
{
	rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

	rapidjson::Value container(rapidjson::kObjectType);

	EntityPropertiesBase::addBaseDataToJsonDocument(container, allocator, "boolean");

	rapidjson::Value jsonValue(rapidjson::kNumberType);
	jsonValue.SetBool(value);
	container.AddMember("Value", jsonValue, allocator);

	rapidjson::Value::StringRefType jsonName(getName().c_str());

	jsonDoc.AddMember(jsonName, container, allocator);
}

void EntityPropertiesBoolean::readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root)
{
	const rapidjson::Value& val = object["Value"];

	setValue(val.GetBool());
}

void EntityPropertiesBoolean::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesBoolean *entity = dynamic_cast<EntityPropertiesBoolean *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue());
	}
}

bool EntityPropertiesBoolean::hasSameValue(EntityPropertiesBase* other)
{
	EntityPropertiesBoolean* entity = dynamic_cast<EntityPropertiesBoolean*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

// ################################################################################################################################################################

void EntityPropertiesString::createProperty(const std::string &group, const std::string &name, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, defaultValue);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesString(name, value), group);
}

void EntityPropertiesString::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyString* newProp = new ot::PropertyString(this->getName(), value);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesString::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{
	const ot::PropertyString* actualProperty = dynamic_cast<const ot::PropertyString*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	setValue(actualProperty->getValue());
}

void EntityPropertiesString::addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root)
{
	rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

	rapidjson::Value container(rapidjson::kObjectType);

	EntityPropertiesBase::addBaseDataToJsonDocument(container, allocator, "string");

	rapidjson::Value jsonValue(value.c_str(), allocator);
	container.AddMember("Value", jsonValue, allocator);

	rapidjson::Value::StringRefType jsonName(getName().c_str());

	jsonDoc.AddMember(jsonName, container, allocator);
}

void EntityPropertiesString::readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root)
{
	const rapidjson::Value& val = object["Value"];

	setValue(val.GetString());
}

void EntityPropertiesString::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesString *entity = dynamic_cast<EntityPropertiesString *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue());
	}
}

bool EntityPropertiesString::hasSameValue(EntityPropertiesBase* other)
{
	EntityPropertiesString* entity = dynamic_cast<EntityPropertiesString*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

// ################################################################################################################################################################

void EntityPropertiesSelection::createProperty(const std::string &group, const std::string &name, std::list<std::string>& options, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, defaultValue);

	// Finally create the new property
	EntityPropertiesSelection *prop = new EntityPropertiesSelection;
	prop->setName(name);

	for (auto& item : options) prop->addOption(item);

	prop->setValue(value);

	properties.createProperty(prop, group);
}

void EntityPropertiesSelection::createProperty(const std::string& group, const std::string& name, std::list<std::string>&& options, const std::string& defaultValue, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, defaultValue);

	// Finally create the new property
	EntityPropertiesSelection* prop = new EntityPropertiesSelection;
	prop->setName(name);

	for (auto& item : options) prop->addOption(item);

	prop->setValue(value);

	properties.createProperty(prop, group);
}

void EntityPropertiesSelection::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), value, options);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesSelection::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{
	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	if (value != actualProperty->getCurrent()) setNeedsUpdate();
	value = actualProperty->getCurrent();

	options.clear();
	for (const std::string& s : actualProperty->getList())
	{
		options.push_back(s);
	}
}

void EntityPropertiesSelection::addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root)
{
	rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

	rapidjson::Value container(rapidjson::kObjectType);

	EntityPropertiesBase::addBaseDataToJsonDocument(container, allocator, "selection");

	rapidjson::Value jsonValue(value.c_str(), allocator);
	rapidjson::Value jsonOptions(rapidjson::kArrayType);

	for (auto option : options)
	{
		rapidjson::Value val(option.c_str(), allocator);
		jsonOptions.PushBack(val, allocator);;
	}

	container.AddMember("Value", jsonValue, allocator);
	container.AddMember("Options", jsonOptions, allocator);

	rapidjson::Value::StringRefType jsonName(getName().c_str());

	jsonDoc.AddMember(jsonName, container, allocator);
}

void EntityPropertiesSelection::readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root)
{
	const rapidjson::Value& val = object["Value"];
	const rapidjson::Value& opt = object["Options"];

	if (value != val.GetString()) setNeedsUpdate();
	value = val.GetString();

	// Now read the options
	assert(opt.IsArray());

	for (rapidjson::SizeType i = 0; i < opt.Size(); i++)
	{
		options.push_back(opt[i].GetString());
	}
}

void EntityPropertiesSelection::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesSelection *entity = dynamic_cast<EntityPropertiesSelection *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		assert(checkCompatibilityOfSettings(*entity));
		setValue(entity->getValue());
	}
}

void EntityPropertiesSelection::resetOptions(std::list<std::string>& _options)
{
	options.clear();
	options.reserve(_options.size());
	for (auto& item : _options)
	{
		addOption(item);
	}
	if (options.size() == 0)
	{
		value = "";
	}
	else
	{
		value = options.front();
	}
}

bool EntityPropertiesSelection::hasSameValue(EntityPropertiesBase* other)
{
	EntityPropertiesSelection* entity = dynamic_cast<EntityPropertiesSelection*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

bool EntityPropertiesSelection::setValue(const std::string& s)
{
	if (std::find(options.begin(), options.end(), s) == options.end()) return false; // This value is not a valid option

	if (value != s) setNeedsUpdate();

	value = s;
	return true;
}

bool EntityPropertiesSelection::checkCompatibilityOfSettings(const EntityPropertiesSelection& other)
{
	if (options.size() != other.options.size()) return false;

	for (int i = 0; i < options.size(); i++)
	{
		if (options[i] != other.options[i]) return false;
	}

	return true;
}

bool EntityPropertiesSelection::isCompatible(EntityPropertiesBase* other)
{
	EntityPropertiesSelection* otherItem = dynamic_cast<EntityPropertiesSelection*>(other);
	if (otherItem == nullptr) return false;

	return checkCompatibilityOfSettings(*otherItem);
}

// ################################################################################################################################################################

void EntityPropertiesColor::createProperty(const std::string &group, const std::string &name, std::vector<int> defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	assert(defaultValue.size() == 3);

	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	int valueR = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor(defaultCategory, name, 0, defaultValue[0]);
	int valueG = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor(defaultCategory, name, 1, defaultValue[1]);
	int valueB = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultColor(defaultCategory, name, 2, defaultValue[2]);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesColor(name, valueR / 255.0, valueG / 255.0, valueB / 255.0), group);
}

void EntityPropertiesColor::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::Color col((int)(getColorR() * 255.), (int)(getColorG() * 255.), (int)(getColorB() * 255.));
	ot::PropertyColor* newProp = new ot::PropertyColor(this->getName(), col);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesColor::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{
	const ot::PropertyColor* actualProperty = dynamic_cast<const ot::PropertyColor*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}
	ot::ColorF c(actualProperty->getValue().toColorF());
	setColorR(c.r());
	setColorG(c.g());
	setColorB(c.b());
}

void EntityPropertiesColor::addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root)
{
	rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

	rapidjson::Value container(rapidjson::kObjectType);

	EntityPropertiesBase::addBaseDataToJsonDocument(container, allocator, "color");

	rapidjson::Value jsonValueR(rapidjson::kNumberType);
	rapidjson::Value jsonValueG(rapidjson::kNumberType);
	rapidjson::Value jsonValueB(rapidjson::kNumberType);

	jsonValueR.SetDouble(getColorR());
	jsonValueG.SetDouble(getColorG());
	jsonValueB.SetDouble(getColorB());

	container.AddMember("ValueR", jsonValueR, allocator);
	container.AddMember("ValueG", jsonValueG, allocator);
	container.AddMember("ValueB", jsonValueB, allocator);

	rapidjson::Value::StringRefType jsonName(getName().c_str());

	jsonDoc.AddMember(jsonName, container, allocator);
}

void EntityPropertiesColor::readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root)
{
	const rapidjson::Value& valR = object["ValueR"];
	const rapidjson::Value& valG = object["ValueG"];
	const rapidjson::Value& valB = object["ValueB"];

	setColorR(valR.GetDouble());
	setColorG(valG.GetDouble());
	setColorB(valB.GetDouble());
}

EntityPropertiesColor& EntityPropertiesColor::operator=(const EntityPropertiesColor &other)
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

void EntityPropertiesColor::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesColor *entity = dynamic_cast<EntityPropertiesColor *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setColorRGB(entity->getColorR(), entity->getColorG(), entity->getColorB());
	}
}

bool EntityPropertiesColor::hasSameValue(EntityPropertiesBase* other)
{
	EntityPropertiesColor* entity = dynamic_cast<EntityPropertiesColor*>(other);

	if (entity == nullptr) return false;

	return (getColorR() == entity->getColorR() && getColorG() == entity->getColorG() && getColorB() == entity->getColorB());
}

// ################################################################################################################################################################

bool EntityPropertiesEntityList::hasSameValue(EntityPropertiesBase *other)
{
	EntityPropertiesEntityList *entity = dynamic_cast<EntityPropertiesEntityList *>(other);

	if (entity == nullptr) return false;

	return (getEntityContainerName() == entity->getEntityContainerName() && getEntityContainerID() == entity->getEntityContainerID() && getValueName() == entity->getValueName() && getValueID() == entity->getValueID());
}

void EntityPropertiesEntityList::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	std::list<std::string> opt;

	if (root != nullptr)
	{
		this->updateValueAndContainer(root, opt);
	}

	if (!root)
	{
		opt.clear();
	}

	ot::JsonDocument dataDoc;
	dataDoc.AddMember("ContainerName", ot::JsonString(this->getEntityContainerName(), dataDoc.GetAllocator()), dataDoc.GetAllocator());
	dataDoc.AddMember("ContainerID", this->getEntityContainerID(), dataDoc.GetAllocator());
	dataDoc.AddMember("ValueID", this->getValueID(), dataDoc.GetAllocator());

	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), this->getValueName(), opt);
	newProp->setSpecialType("EntityList");
	newProp->addAdditionalPropertyData("EntityData", dataDoc.toJson());
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesEntityList::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{
	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	std::string entityData = actualProperty->getAdditionalPropertyData("EntityData");
	if (entityData.empty()) {
		OT_LOG_E("Data missing");
		return;
	}

	ot::JsonDocument dataDoc;
	dataDoc.fromJson(entityData);

	//this->setValueName(ot::json::getString(dataDoc, "ValueName"));
	this->setValueName(actualProperty->getCurrent());
	this->setEntityContainerName(ot::json::getString(dataDoc, "ContainerName"));
	this->setEntityContainerID(ot::json::getUInt64(dataDoc, "ContainerID"));
	this->setValueID(ot::json::getUInt64(dataDoc, "ValueID"));

	if (root) {
		std::list<std::string> opt;
		this->updateValueAndContainer(root, opt);
	}
	
}

void EntityPropertiesEntityList::addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root)
{
	ot::JsonAllocator& allocator = jsonDoc.GetAllocator();

	rapidjson::Value container(rapidjson::kObjectType);

	EntityPropertiesBase::addBaseDataToJsonDocument(container, allocator, "entitylist");

	rapidjson::Value jsonContainerName(rapidjson::kStringType);
	rapidjson::Value jsonContainerID(rapidjson::kNumberType);
	rapidjson::Value jsonValueName(rapidjson::kStringType);
	rapidjson::Value jsonValueID(rapidjson::kNumberType);
	std::list<std::string> opt;

	if (root) {
		this->updateValueAndContainer(root, opt);
	}

	jsonContainerName.SetString(this->getEntityContainerName().c_str(), allocator);
	jsonContainerID.SetInt64(this->getEntityContainerID());
	jsonValueName.SetString(this->getValueName().c_str(), allocator);
	jsonValueID.SetInt64(this->getValueID());

	container.AddMember("ContainerName", jsonContainerName, allocator);
	container.AddMember("ContainerID", jsonContainerID, allocator);
	container.AddMember("ValueName", jsonValueName, allocator);
	container.AddMember("ValueID", jsonValueID, allocator);

	if (root != nullptr)
	{
		container.AddMember("Options", ot::JsonArray(opt, allocator), allocator);
	}

	rapidjson::Value::StringRefType jsonName(getName().c_str());

	jsonDoc.AddMember(jsonName, container, allocator);
}

void EntityPropertiesEntityList::readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root)
{
	const rapidjson::Value& containerName = object["ContainerName"];
	const rapidjson::Value& containerID = object["ContainerID"];
	const rapidjson::Value& valName = object["ValueName"];
	const rapidjson::Value& valID = object["ValueID"];

	this->setEntityContainerName(containerName.GetString());
	this->setEntityContainerID(containerID.GetInt64());
	this->setValueName(valName.GetString());
	this->setValueID(valID.GetInt64());

	if (root) {
		std::list<std::string> opt;
		this->updateValueAndContainer(root, opt);
	}
}

EntityPropertiesEntityList& EntityPropertiesEntityList::operator=(const EntityPropertiesEntityList &other)
{
	if (&other != this)
	{
		EntityPropertiesBase::operator=(other);

		setEntityContainerName(other.getEntityContainerName()); 
		setEntityContainerID(other.getEntityContainerID()); 
		setValueName(other.getValueName()); 
		setValueID(other.getValueID());
	}

	return *this;
}

void EntityPropertiesEntityList::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	// Here we can either copy the entire settings from another EntityPropertiesEntityList
	// or alternatively from a EntityPropertiesSelection. The latter is required when we read
	// the property change from a UI

	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesEntityList *entity = dynamic_cast<EntityPropertiesEntityList *>(other);
	if (entity != nullptr)
	{
		setEntityContainerName(entity->getEntityContainerName());
		setEntityContainerID(entity->getEntityContainerID());
		setValueName(entity->getValueName());
		setValueID(entity->getValueID());

		return;
	}

	EntityPropertiesSelection *selection = dynamic_cast<EntityPropertiesSelection *>(other);
	assert(selection != nullptr);
	assert(root != nullptr);

	if (selection != nullptr)
	{
		// We keep our current settings for the entity container
		valueName = selection->getValue();

		// Now we need to search for the entity ID for the selected name
		EntityBase *entity = findEntityFromName(root, valueName);
		assert(entity != nullptr);

		if (entity != nullptr)
		{
			setValueID(entity->getEntityID());
		}

		// Now we update the container ID
		EntityContainer *container = findContainerFromID(root, getEntityContainerID());
		if (container != nullptr)
		{
			// The entity id still exists, so update the name if necessary
			setEntityContainerName(container->getName());
		}
		else
		{
			// The container id does not exist, try to find the container by its name
			container = findContainerFromName(root, getEntityContainerName());

			if (container != nullptr)
			{
				setEntityContainerID(container->getEntityID());
			}
		}
	}
}

void EntityPropertiesEntityList::createProperty(const std::string& group, const std::string& name, const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, valName);

	// Finally create the new property
	properties.createProperty(new EntityPropertiesEntityList(name, contName, contID, value, valID), group);
}

void EntityPropertiesEntityList::updateValueAndContainer(EntityBase* _root, std::list<std::string>& _containerOptions) {
	OT_LOG_D("DEBUG");
	OTAssertNullptr(_root);

	EntityContainer* container = this->findContainerFromID(_root, getEntityContainerID());
	if (container) {
		this->setEntityContainerName(container->getName());
	}
	else {
		container = this->findContainerFromName(_root, getEntityContainerName());
		if (container) {
			this->setEntityContainerID(container->getEntityID());
		}
	}

	if (container) {
		for (auto child : container->getChildrenList()) {
			_containerOptions.push_back(child->getName());
		}
	}
	else {
		OT_LOG_EA("Container not found");
	}

	EntityBase* entity = this->findEntityFromID(_root, this->getValueID());

	if (entity != nullptr) {
		this->setValueName(entity->getName());
	}
	else {
		entity = this->findEntityFromName(_root, this->getValueName());
		if (entity != nullptr) {
			this->setValueID(entity->getEntityID());
		}
		else {
			OT_LOG_EA("Value not found");
		}
	}
}

EntityContainer* EntityPropertiesEntityList::findContainerFromID(EntityBase* root, ot::UID entityID)
{
	EntityContainer* container = dynamic_cast<EntityContainer*>(root);

	if (container != nullptr)
	{
		if (container->getEntityID() == entityID) return container;

		for (auto child : container->getChildrenList())
		{
			EntityContainer* containerChild = findContainerFromID(child, entityID);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

EntityContainer* EntityPropertiesEntityList::findContainerFromName(EntityBase* root, const std::string& entityName)
{
	EntityContainer* container = dynamic_cast<EntityContainer*>(root);

	if (container != nullptr)
	{
		if (container->getName() == entityName) return container;

		for (auto child : container->getChildrenList())
		{
			EntityContainer* containerChild = findContainerFromName(child, entityName);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

EntityBase *EntityPropertiesEntityList::findEntityFromName(EntityBase *root, const std::string &entityName)
{
	if (root->getName() == entityName) return root;

	EntityContainer *container = dynamic_cast<EntityContainer *>(root);

	if (container != nullptr)
	{
		for (auto child : container->getChildrenList())
		{
			EntityBase *containerChild = findEntityFromName(child, entityName);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

EntityBase *EntityPropertiesEntityList::findEntityFromID(EntityBase *root, ot::UID entityID)
{
	if (root->getEntityID() == entityID) return root;

	EntityContainer *container = dynamic_cast<EntityContainer *>(root);

	if (container != nullptr)
	{
		for (auto child : container->getChildrenList())
		{
			EntityBase *containerChild = findEntityFromID(child, entityID);
			if (containerChild != nullptr) return containerChild;
		}
	}

	return nullptr;
}

// ################################################################################################################################################################

void EntityPropertiesProjectList::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);
	EntityPropertiesProjectList* otherProject = dynamic_cast<EntityPropertiesProjectList*>(other);
	_value = otherProject->_value;
	setNeedsUpdate();
}

void EntityPropertiesProjectList::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), _value, std::list<std::string>());
	newProp->setSpecialType("ProjectList");
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesProjectList::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{
	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	_value = actualProperty->getCurrent();
}

void EntityPropertiesProjectList::addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root)
{
	rapidjson::Value container(rapidjson::kObjectType);
	EntityPropertiesBase::addBaseDataToJsonDocument(container, jsonDoc.GetAllocator(), "projectlist");

	rapidjson::Value jsonValue(rapidjson::kStringType);
	jsonValue.SetString(_value.c_str(), jsonDoc.GetAllocator());
	container.AddMember("Value", jsonValue, jsonDoc.GetAllocator());
	rapidjson::Value::StringRefType jsonName(getName().c_str());
	jsonDoc.AddMember(jsonName, container, jsonDoc.GetAllocator());

}

void EntityPropertiesProjectList::readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root)
{
	_value = ot::json::getString(object, "Value");
}

// ################################################################################################################################################################

EntityPropertiesGuiPainter::EntityPropertiesGuiPainter() {
	m_painter = new ot::FillPainter2D;
}

EntityPropertiesGuiPainter::~EntityPropertiesGuiPainter() {
	delete m_painter;
	m_painter = nullptr;
}

EntityPropertiesGuiPainter::EntityPropertiesGuiPainter(const EntityPropertiesGuiPainter& other) 
	: EntityPropertiesBase(other), m_painter(nullptr)
{
	m_painter = other.getValue()->createCopy();
}

EntityPropertiesGuiPainter& EntityPropertiesGuiPainter::operator=(const EntityPropertiesGuiPainter& other) {
	if (&other != this)
	{
		EntityPropertiesBase::operator=(other);
		setValue(other.getValue()->createCopy());
	}

	return *this;
}

void EntityPropertiesGuiPainter::setValue(ot::Painter2D* _painter) {
	if (m_painter == _painter) return;
	OTAssertNullptr(_painter);
	OTAssertNullptr(m_painter);
	if (m_painter->isEqualTo(_painter)) return;

	delete m_painter;
	m_painter = _painter->createCopy();
	this->setNeedsUpdate();
}

void EntityPropertiesGuiPainter::createProperty(const std::string& group, const std::string& name, ot::Painter2D* _defaultPainter, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	const ot::Painter2D* newPainter = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultGuiPainter(defaultCategory, name);

	if (newPainter) {
		if (_defaultPainter) delete _defaultPainter;
		_defaultPainter = newPainter->createCopy();
	}

	// Finally create the new property
	properties.createProperty(new EntityPropertiesGuiPainter(name, _defaultPainter), group);
}

void EntityPropertiesGuiPainter::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	OTAssertNullptr(m_painter);
	ot::PropertyPainter2D* newProp = new ot::PropertyPainter2D(this->getName(), m_painter->createCopy());
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesGuiPainter::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{
	const ot::PropertyPainter2D* actualProperty = dynamic_cast<const ot::PropertyPainter2D*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	this->setValue(actualProperty->getPainter()->createCopy());
}

void EntityPropertiesGuiPainter::addToJsonDocument(ot::JsonDocument& jsonDoc, EntityBase* root)
{
	ot::JsonObject container;
	EntityPropertiesBase::addBaseDataToJsonDocument(container, jsonDoc.GetAllocator(), "guipainter");

	ot::JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, jsonDoc.GetAllocator());
	container.AddMember("Value", painterObj, jsonDoc.GetAllocator());

	rapidjson::Value::StringRefType jsonName(getName().c_str());

	jsonDoc.AddMember(ot::JsonString(this->getName(), jsonDoc.GetAllocator()), container, jsonDoc.GetAllocator());
}

void EntityPropertiesGuiPainter::readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root)
{
	ot::Painter2D* newPainter = ot::Painter2DFactory::instance().createFromJSON(ot::json::getObject(object, "Value"), OT_JSON_MEMBER_Painter2DType);
	if (!newPainter) return;

	setValue(newPainter);
}

void EntityPropertiesGuiPainter::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesGuiPainter* entity = dynamic_cast<EntityPropertiesGuiPainter*>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue()->createCopy());
	}
}

bool EntityPropertiesGuiPainter::hasSameValue(EntityPropertiesBase* other)
{
	EntityPropertiesGuiPainter* entity = dynamic_cast<EntityPropertiesGuiPainter*>(other);

	if (entity == nullptr) return false;

	return (getValue()->isEqualTo(entity->getValue()));
}
