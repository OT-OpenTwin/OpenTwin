// @otlicense
// File: EntityPropertiesItems.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include <cassert>

#include "EntityPropertiesItems.h"
#include "EntityProperties.h"
#include "EntityContainer.h"
#include "TemplateDefaultManager.h"

#include "OTCore/LogDispatcher.h"
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

EntityPropertiesBase::EntityPropertiesBase()
	: m_container(nullptr), m_needsUpdateFlag(false), m_multipleValues(false), m_readOnly(false), m_protectedProperty(true),
	m_visible(true), m_errorState(false) 
{}

EntityPropertiesBase::EntityPropertiesBase(const EntityPropertiesBase &other)
{ 
	m_name = other.m_name;
	m_group = other.m_group;

	m_multipleValues = other.m_multipleValues;
	m_container = other.m_container;
	m_needsUpdateFlag = other.m_needsUpdateFlag;
	m_readOnly = other.m_readOnly;
	m_protectedProperty = other.m_protectedProperty;
	m_visible = other.m_visible;
	m_errorState = other.m_errorState;
	m_toolTip = other.m_toolTip;
}

void EntityPropertiesBase::setNeedsUpdate() {
	if (m_container != nullptr) m_container->setNeedsUpdate();

	m_needsUpdateFlag = true;
}

void EntityPropertiesBase::setFromConfiguration(const ot::Property* _property, EntityBase* root) {
	this->setName(_property->getPropertyName());
	this->setHasMultipleValues(_property->getPropertyFlags() & ot::Property::HasMultipleValues);
	this->setReadOnly(_property->getPropertyFlags() & ot::Property::IsReadOnly);
	this->setProtected(!(_property->getPropertyFlags() & ot::Property::IsDeletable));
	this->setVisible(!(_property->getPropertyFlags() & ot::Property::IsHidden));
	this->setErrorState(_property->getPropertyFlags() & ot::Property::HasInputError);
	this->setToolTip(_property->getPropertyTip());
}

void EntityPropertiesBase::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* root) {
	_jsonObject.AddMember("Type", ot::JsonString(this->getTypeString(), _allocator), _allocator);
	_jsonObject.AddMember("MultipleValues", this->hasMultipleValues(), _allocator);
	_jsonObject.AddMember("ReadOnly", this->getReadOnly(), _allocator);
	_jsonObject.AddMember("Protected", this->getProtected(), _allocator);
	_jsonObject.AddMember("Visible", this->getVisible(), _allocator);
	_jsonObject.AddMember("ErrorState", this->getErrorState(), _allocator);
	_jsonObject.AddMember("Group", ot::JsonString(this->getGroup(), _allocator), _allocator);
	_jsonObject.AddMember("ToolTip", ot::JsonString(this->getToolTip(), _allocator), _allocator);
}

void EntityPropertiesBase::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root) {
	if (_object.HasMember("MultipleValues")) {
		this->setHasMultipleValues(ot::json::getBool(_object, "MultipleValues"));
	}
	if (_object.HasMember("ReadOnly")) {
		this->setReadOnly(ot::json::getBool(_object, "ReadOnly"));
	}
	if (_object.HasMember("Protected")) {
		this->setProtected(ot::json::getBool(_object, "Protected"));
	}
	if (_object.HasMember("Visible")) {
		this->setVisible(ot::json::getBool(_object, "Visible"));
	}
	if (_object.HasMember("ErrorState")) {
		this->setErrorState(ot::json::getBool(_object, "ErrorState"));
	}
	if (_object.HasMember("ToolTip")) {
		this->setToolTip(ot::json::getString(_object, "ToolTip"));
	}
}

void EntityPropertiesBase::copySettings(EntityPropertiesBase *other, EntityBase *root)
{ 
	assert(m_name == other->getName());

	// We keep the container unchanged, since we don't want to overwrite the ownership of this item
	// We also dont assign the group, since the group assignment of a property can not be changed
	m_multipleValues = other->m_multipleValues;
	m_needsUpdateFlag = other->m_needsUpdateFlag;
	m_readOnly = other->m_readOnly;
	m_protectedProperty = other->m_protectedProperty;
	m_visible = other->m_visible;
	m_errorState = other->m_errorState;
	m_toolTip = other->m_toolTip;
}

EntityPropertiesBase& EntityPropertiesBase::operator=(const EntityPropertiesBase &other)
{ 
	if (&other != this) 
	{ 
		// We keep the container unchanged, since we don't want to overwrite the ownership of this item
		m_name = other.m_name;
		m_group = other.m_group;
		m_multipleValues = other.m_multipleValues;
		m_needsUpdateFlag = other.m_needsUpdateFlag;
		m_readOnly = other.m_readOnly;
		m_protectedProperty = other.m_protectedProperty;
		m_visible = other.m_visible;
		m_errorState = other.m_errorState;
		m_toolTip = other.m_toolTip;
	}
	
	return *this; 
}

void EntityPropertiesBase::setupPropertyData(ot::PropertyGridCfg& _configuration, ot::Property* _property) const
{
	if (this->hasMultipleValues()) _property->setPropertyFlag(ot::Property::HasMultipleValues);
	if (this->getReadOnly()) _property->setPropertyFlag(ot::Property::IsReadOnly);
	if (!this->getProtected()) _property->setPropertyFlag(ot::Property::IsDeletable);
	if (!this->getVisible()) _property->setPropertyFlag(ot::Property::IsHidden);
	if (this->getErrorState()) _property->setPropertyFlag(ot::Property::HasInputError);
	_property->setPropertyTip(m_toolTip);

	ot::PropertyGroup* group = _configuration.findOrCreateGroup(this->getGroup());
	OTAssertNullptr(group);
	group->addProperty(_property);
}

bool EntityPropertiesBase::needsUpdate()
{
	return m_needsUpdateFlag;
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
	m_suffix = _other.m_suffix;
}

EntityPropertiesDouble& EntityPropertiesDouble::operator=(const EntityPropertiesDouble& _other) {
	if (&_other != this) {
		EntityPropertiesBase::operator=(_other);
		m_value = _other.m_value;
		m_allowCustomValues = _other.m_allowCustomValues;
		m_min = _other.m_min;
		m_max = _other.m_max;
		m_suffix = _other.m_suffix;
	}
	return *this;
}

void EntityPropertiesDouble::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyDouble* newProp = new ot::PropertyDouble(this->getName(), m_value);
	newProp->setPropertyFlag((m_allowCustomValues ? ot::Property::AllowCustomValues : ot::Property::NoFlags));
	newProp->setRange(m_min, m_max);
	newProp->setSuffix(m_suffix);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesDouble::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyDouble* actualProperty = dynamic_cast<const ot::PropertyDouble *>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	this->setValue(actualProperty->getValue());
	this->setRange(actualProperty->getMin(), actualProperty->getMax());
	this->setSuffix(actualProperty->getSuffix());
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
}

void EntityPropertiesDouble::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);
	this->setValue(ot::json::getDouble(_object, "Value"));

	if (_object.HasMember("MinValue")) {
		this->setMin(ot::json::getDouble(_object, "MinValue", std::numeric_limits<double>::lowest()));
	}
	if (_object.HasMember("MaxValue")) {
		this->setMax(ot::json::getDouble(_object, "MaxValue", std::numeric_limits<double>::max()));
	}
	if (_object.HasMember("AllowCustom")) {
		this->setAllowCustomValues(ot::json::getBool(_object, "AllowCustom", true));
	}
	if (_object.HasMember("Suffix")) {
		this->setSuffix(ot::json::getString(_object, "Suffix"));
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
		this->setSuffix(entity->getSuffix());
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

bool EntityPropertiesDouble::hasSameValue(EntityPropertiesBase* other) const {
	EntityPropertiesDouble* entity = dynamic_cast<EntityPropertiesDouble*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

EntityPropertiesDouble* EntityPropertiesDouble::createProperty(const std::string& _group, const std::string& _name, double _defaultValue, const std::string& _defaultCategory, EntityProperties& _properties) {
	return EntityPropertiesDouble::createProperty(_group, _name, _defaultValue, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), _defaultCategory, _properties);
}

EntityPropertiesDouble* EntityPropertiesDouble::createProperty(const std::string& _group, const std::string& _name, double _defaultValue, double _minValue, double _maxValue, const std::string& _defaultCategory, EntityProperties& _properties) {
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
	m_suffix = _other.m_suffix;
}

EntityPropertiesInteger& EntityPropertiesInteger::operator=(const EntityPropertiesInteger& _other) {
	if (&_other != this) {
		EntityPropertiesBase::operator=(_other);
		m_value = _other.getValue();
		m_allowCustomValues = _other.m_allowCustomValues;
		m_min = _other.m_min;
		m_max = _other.m_max;
		m_suffix = _other.m_suffix;
	}
	return *this;
}

void EntityPropertiesInteger::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
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
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	this->setValue(actualProperty->getValue());
	this->setRange(actualProperty->getMin(), actualProperty->getMax());
	this->setSuffix(actualProperty->getSuffix());
	this->setAllowCustomValues(actualProperty->getPropertyFlags() & ot::PropertyBase::AllowCustomValues);
}

void EntityPropertiesInteger::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) {
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
	this->setValue((long) ot::json::getInt64(_object, "Value"));
	if (_object.HasMember("MinValue")) {
		this->setMin((long) ot::json::getInt64(_object, "MinValue", std::numeric_limits<long>::lowest()));
	}
	if (_object.HasMember("MaxValue")) {
		this->setMax((long) ot::json::getInt64(_object, "MaxValue", std::numeric_limits<long>::max()));
	}
	if (_object.HasMember("AllowCustom")) {
		this->setAllowCustomValues(ot::json::getBool(_object, "AllowCustom", true));
	}
	if (_object.HasMember("Suffix")) {
		this->setSuffix(ot::json::getString(_object, "Suffix"));
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

bool EntityPropertiesInteger::hasSameValue(EntityPropertiesBase* other) const
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
		this->setSuffix(entity->getSuffix());
	}
}

EntityPropertiesInteger* EntityPropertiesInteger::createProperty(const std::string& _group, const std::string& _name, long _defaultValue, const std::string& _defaultCategory, EntityProperties& _properties) {
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(_defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	long value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultLong(_defaultCategory, _name, _defaultValue);

	// Finally create the new property
	EntityPropertiesInteger* newProperty = new EntityPropertiesInteger(_name, value);
	_properties.createProperty(newProperty, _group);
	return newProperty;
}

EntityPropertiesInteger* EntityPropertiesInteger::createProperty(const std::string& _group, const std::string& _name, long _defaultValue, long _minValue, long _maxValue, const std::string& _defaultCategory, EntityProperties& _properties) {
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

// ################################################################################################################################################################

EntityPropertiesBoolean* EntityPropertiesBoolean::createProperty(const std::string &group, const std::string &name, bool defaultValue, const std::string &defaultCategory, EntityProperties &properties)
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

void EntityPropertiesBoolean::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyBool* newProp = new ot::PropertyBool(this->getName(), m_value);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesBoolean::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyBool* actualProperty = dynamic_cast<const ot::PropertyBool*>(_property);
	if (!actualProperty) {
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

bool EntityPropertiesBoolean::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesBoolean* entity = dynamic_cast<EntityPropertiesBoolean*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

// ################################################################################################################################################################

EntityPropertiesString* EntityPropertiesString::createProperty(const std::string &group, const std::string &name, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, defaultValue);

	// Finally create the new property
	EntityPropertiesString* newProperty = new EntityPropertiesString(name, value);
	newProperty->setIsMultiline(TemplateDefaultManager::getTemplateDefaultManager()->getDefaultBool(defaultCategory, name + "_Multiline", false));
	properties.createProperty(newProperty, group);
	return newProperty;
}

void EntityPropertiesString::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyString* newProp = new ot::PropertyString(this->getName(), m_value);
	this->setupPropertyData(_configuration, newProp);
	newProp->setMultiline(m_isMultiline);
}

void EntityPropertiesString::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyString* actualProperty = dynamic_cast<const ot::PropertyString*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	setValue(actualProperty->getValue());
	setIsMultiline(actualProperty->isMultiline());
}

void EntityPropertiesString::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);
	_jsonObject.AddMember("Value", ot::JsonString(m_value, _allocator), _allocator);
	_jsonObject.AddMember("IsMultiline", m_isMultiline, _allocator);
}

void EntityPropertiesString::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);
	this->setValue(ot::json::getString(_object, "Value"));
	if (_object.HasMember("IsMultiline")) {
		this->setIsMultiline(ot::json::getBool(_object, "IsMultiline", false));
	}
}

void EntityPropertiesString::copySettings(EntityPropertiesBase *other, EntityBase *root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesString *entity = dynamic_cast<EntityPropertiesString *>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue());
		setIsMultiline(entity->getIsMultiline());
	}
}

bool EntityPropertiesString::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesString* entity = dynamic_cast<EntityPropertiesString*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

// ################################################################################################################################################################

EntityPropertiesSelection* EntityPropertiesSelection::createProperty(const std::string &group, const std::string &name, std::list<std::string>& options, const std::string &defaultValue, const std::string &defaultCategory, EntityProperties &properties)
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

	return prop;
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

	for (auto& item : options) prop->addOption(item);

	prop->setValue(value);

	properties.createProperty(prop, group);

	return prop;
}

void EntityPropertiesSelection::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::PropertyStringList* newProp = new ot::PropertyStringList(this->getName(), m_value, m_options);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesSelection::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	if (m_value != actualProperty->getCurrent()) setNeedsUpdate();
	m_value = actualProperty->getCurrent();

	m_options.clear();
	for (const std::string& s : actualProperty->getList())
	{
		m_options.push_back(s);
	}
}

void EntityPropertiesSelection::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);

	_jsonObject.AddMember("Value", ot::JsonString(m_value, _allocator), _allocator);
	_jsonObject.AddMember("Options", ot::JsonArray(m_options, _allocator), _allocator);
}

void EntityPropertiesSelection::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);

	const rapidjson::Value& val = _object["Value"];
	const rapidjson::Value& opt = _object["Options"];

	if (m_value != val.GetString()) setNeedsUpdate();
	m_value = val.GetString();

	// Now read the options
	assert(opt.IsArray());

	for (rapidjson::SizeType i = 0; i < opt.Size(); i++)
	{
		m_options.push_back(opt[i].GetString());
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

void EntityPropertiesSelection::resetOptions(const std::list<std::string>& _options)
{
	m_options.clear();
	m_options.reserve(_options.size());
	for (auto& item : _options)
	{
		addOption(item);
	}
	if (m_options.size() == 0)
	{
		m_value = "";
	}
	else
	{
		m_value = m_options.front();
	}
}

bool EntityPropertiesSelection::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesSelection* entity = dynamic_cast<EntityPropertiesSelection*>(other);

	if (entity == nullptr) return false;

	return (getValue() == entity->getValue());
}

bool EntityPropertiesSelection::setValue(const std::string& s)
{
	if (std::find(m_options.begin(), m_options.end(), s) == m_options.end()) return false; // This value is not a valid option

	if (m_value != s) setNeedsUpdate();

	m_value = s;
	return true;
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

// ################################################################################################################################################################

EntityPropertiesColor* EntityPropertiesColor::createProperty(const std::string &group, const std::string &name, std::vector<int> defaultValue, const std::string &defaultCategory, EntityProperties &properties)
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

void EntityPropertiesColor::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase *root)
{
	ot::Color col((int)(getColorR() * 255.), (int)(getColorG() * 255.), (int)(getColorB() * 255.));
	ot::PropertyColor* newProp = new ot::PropertyColor(this->getName(), col);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesColor::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
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

void EntityPropertiesColor::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) {
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

bool EntityPropertiesColor::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesColor* entity = dynamic_cast<EntityPropertiesColor*>(other);

	if (entity == nullptr) return false;

	return (getColorR() == entity->getColorR() && getColorG() == entity->getColorG() && getColorB() == entity->getColorB());
}

// ################################################################################################################################################################

bool EntityPropertiesEntityList::hasSameValue(EntityPropertiesBase *other) const
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

void EntityPropertiesEntityList::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
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

	if (_root) {
		std::list<std::string> opt;
		this->updateValueAndContainer(_root, opt);
	}
	
}

void EntityPropertiesEntityList::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);

	std::list<std::string> opt;

	if (_root) {
		this->updateValueAndContainer(_root, opt);
	}

	_jsonObject.AddMember("ContainerName", ot::JsonString(this->getEntityContainerName(), _allocator), _allocator);
	_jsonObject.AddMember("ContainerID", static_cast<int64_t>(this->getEntityContainerID()), _allocator);
	_jsonObject.AddMember("ValueName", ot::JsonString(this->getValueName(), _allocator), _allocator);
	_jsonObject.AddMember("ValueID", static_cast<int64_t>(this->getValueID()), _allocator);

	if (_root) {
		_jsonObject.AddMember("Options", ot::JsonArray(opt, _allocator), _allocator);
	}
}

void EntityPropertiesEntityList::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);

	const rapidjson::Value& containerName = _object["ContainerName"];
	const rapidjson::Value& containerID = _object["ContainerID"];
	const rapidjson::Value& valName = _object["ValueName"];
	const rapidjson::Value& valID = _object["ValueID"];

	this->setEntityContainerName(containerName.GetString());
	this->setEntityContainerID(containerID.GetInt64());
	this->setValueName(valName.GetString());
	this->setValueID(valID.GetInt64());

	if (_root) {
		std::list<std::string> opt;
		this->updateValueAndContainer(_root, opt);
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
		m_valueName = selection->getValue();

		// Now we need to search for the entity ID for the selected name
		EntityBase *entity = findEntityFromName(root, m_valueName);
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

EntityPropertiesEntityList* EntityPropertiesEntityList::createProperty(const std::string& group, const std::string& name, const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	std::string value = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultString(defaultCategory, name, valName);

	// Finally create the new property
	EntityPropertiesEntityList* newProperty = new EntityPropertiesEntityList(name, contName, contID, value, valID);
	properties.createProperty(newProperty, group);
	return newProperty;
}

void EntityPropertiesEntityList::updateValueAndContainer(EntityBase* _root, std::list<std::string>& _containerOptions) {
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
		//OT_LOG_EA("Container not found"); For copied items, e.g. mesh data items, this information is now available, so the current information in the entity should be used.
		//							        No error message should be shown in this case.
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
			//OT_LOG_EA("Value not found");  For copied items, e.g. mesh data items, this information is now available, so the current information in the entity should be used.
			//							     No error message should be shown in this case.
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
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
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
	m_filter = other.m_filter;
}

EntityPropertiesGuiPainter& EntityPropertiesGuiPainter::operator=(const EntityPropertiesGuiPainter& other) {
	if (&other != this)
	{
		EntityPropertiesBase::operator=(other);
		setValue(other.getValue()->createCopy());
		m_filter = other.m_filter;
	}

	return *this;
}

void EntityPropertiesGuiPainter::setValue(const ot::Painter2D* _painter) {
	if (m_painter == _painter) 
	{
		return;
	}
	OTAssertNullptr(_painter);
	OTAssertNullptr(m_painter);
	if (m_painter->isEqualTo(_painter)) 
	{
		return;
	}

	delete m_painter;
	m_painter = _painter->createCopy();
	this->setNeedsUpdate();
}

EntityPropertiesGuiPainter* EntityPropertiesGuiPainter::createProperty(const std::string& group, const std::string& name, ot::Painter2D* _defaultPainter, const std::string& defaultCategory, EntityProperties& properties) {
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	const ot::Painter2D* newPainter = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultGuiPainter(defaultCategory, name);

	if (newPainter) {
		if (_defaultPainter) delete _defaultPainter;
		_defaultPainter = newPainter->createCopy();
	}

	// Finally create the new property
	EntityPropertiesGuiPainter* newProperty = new EntityPropertiesGuiPainter(name, _defaultPainter);
	properties.createProperty(newProperty, group);
	return newProperty;
}

void EntityPropertiesGuiPainter::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	OTAssertNullptr(m_painter);
	ot::PropertyPainter2D* newProp = new ot::PropertyPainter2D(this->getName(), m_painter->createCopy());
	newProp->setFilter(m_filter);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesGuiPainter::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyPainter2D* actualProperty = dynamic_cast<const ot::PropertyPainter2D*>(_property);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return;
	}

	m_filter = actualProperty->getFilter();
	this->setValue(actualProperty->getPainter()->createCopy());
}

void EntityPropertiesGuiPainter::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);

	_jsonObject.AddMember("Value", ot::JsonObject(m_painter, _allocator), _allocator);
	_jsonObject.AddMember("Filter", ot::JsonObject(m_filter, _allocator), _allocator);
}

void EntityPropertiesGuiPainter::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);

	ot::Painter2D* newPainter = ot::Painter2DFactory::create(ot::json::getObject(_object, "Value"));
	if (!newPainter) {
		return;
	}

	if (_object.HasMember("Filter")) {
		m_filter.setFromJsonObject(ot::json::getObject(_object, "Filter"));
	}

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
		m_filter = entity->m_filter;
	}
}

bool EntityPropertiesGuiPainter::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesGuiPainter* entity = dynamic_cast<EntityPropertiesGuiPainter*>(other);

	if (entity == nullptr) return false;

	return (getValue()->isEqualTo(entity->getValue()));
}
