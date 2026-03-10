// @otlicense
// File: PropertyHelper.cpp
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

#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/PropertyHelper.h"
#include "OTCore/Logging/LogDispatcher.h"

// ###########################################################################################################################################################################################################################################################################################################################

// General

bool PropertyHelper::hasProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	auto allProperties = _base->getProperties().getListOfAllProperties();	
	for (auto property : allProperties)
	{
		if (property->getName() == _name)
		{
			if (_groupName == "")
			{
				return true;
			}
			else if (_groupName == property->getGroup())
			{
				return true;
			}
		}
	}
	return false;
}

EntityPropertiesBase* PropertyHelper::getPropertyBase(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		std::string msg = "Failed to access property { \"Name\": \"" + _name + "\", \"Group\": \"" + _groupName + "\"}";
		throw ot::Exception::ObjectNotFound(msg);
	}

	return propertyBase;
}

const EntityPropertiesBase* PropertyHelper::getPropertyBase(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesBase* propertyBase = _base->getProperties().getProperty(_name, _groupName);
	if (propertyBase == nullptr)
	{
		std::string msg = "Failed to access property { \"Name\": \"" + _name + "\", \"Group\": \"" + _groupName + "\"}";
		throw ot::Exception::ObjectNotFound(msg);
	}

	return propertyBase;
}

void PropertyHelper::setPropertyVisible(bool _visible, EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	getPropertyBase(_base, _name, _groupName)->setVisible(_visible);
}

bool PropertyHelper::getPropertyVisible(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	return getPropertyBase(_base, _name, _groupName)->getVisible();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Value getter

double PropertyHelper::getDoublePropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesDouble* doubleProperty = getDoubleProperty(_base, _name, _groupName);
	return doubleProperty->getValue();
}

std::string PropertyHelper::getStringPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesString* stringProperty = getStringProperty(_base, _name, _groupName);
	return stringProperty->getValue();
}

std::string PropertyHelper::getSelectionPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesSelection* selectionProperty = getSelectionProperty(_base, _name, _groupName);
	return selectionProperty->getValue();
}

bool PropertyHelper::getBoolPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesBoolean* boolProperty = getBoolProperty(_base, _name, _groupName);
	return boolProperty->getValue();
}

ot::Color PropertyHelper::getColourPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesColor* colourProperty = getColourProperty(_base, _name, _groupName);

	const int gridColorR = static_cast<int>(colourProperty->getColorR() * 255.0 + 0.5);
	const int gridColorG = static_cast<int>(colourProperty->getColorG() * 255.0 + 0.5);
	const int gridColorB = static_cast<int>(colourProperty->getColorB() * 255.0 + 0.5);
	const ot::Color colour(gridColorR, gridColorG, gridColorB);

	return colour;
}

const ot::Painter2D* PropertyHelper::getPainterPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesGuiPainter* painterProperty = getPainterProperty(_base, _name, _groupName);
	return painterProperty->getValue();
}

int32_t PropertyHelper::getIntegerPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesInteger* intProperty = getIntegerProperty(_base, _name, _groupName);
	return intProperty->getValue();
}

std::string PropertyHelper::getProjectPropertyValue(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesProjectList* projectProperty = getEntityProjectListProperty(_base, _name, _groupName);
	const std::string selectedProjectName = projectProperty->getValue();
	return selectedProjectName;
}

std::string PropertyHelper::getEntityListPropertyValueName(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesEntityList* entityList = getEntityListProperty(_base, _name, _groupName);
	const std::string valueName = entityList->getValueName();
	return valueName;
}

ot::UID PropertyHelper::getEntityListPropertyValueID(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	const EntityPropertiesEntityList* entityList = getEntityListProperty(_base, _name, _groupName);
	const ot::UID valueID = entityList->getValueID();
	return valueID;
}

const EntityPropertiesProjectList* PropertyHelper::getEntityProjectListProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	return getProperty<EntityPropertiesProjectList>(_base, _name, _groupName);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Value setter

void PropertyHelper::setDoublePropertyValue(double _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesDouble* doubleProperty = getDoubleProperty(_base, _name, _groupName);
	doubleProperty->setValue(_value);
}

void PropertyHelper::setStringPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesString* stringProperty = getStringProperty(_base, _name, _groupName);
	stringProperty->setValue(_value);
}

void PropertyHelper::setSelectionPropertyValue(const std::string& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesSelection* selectionProperty = getSelectionProperty(_base, _name, _groupName);
	const std::vector<std::string> options = selectionProperty->getOptions();
	bool valueIsNoOption = std::find(options.begin(), options.end(), _value) == options.end();
	if (valueIsNoOption) {
		throw std::exception("Tried to set a value that is not an option in the selection entity.");
	}

	selectionProperty->setValue(_value);
}

void PropertyHelper::setSelectionPropertyValue(const std::list<std::string>& _values, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesSelection* selectionProperty = getSelectionProperty(_base, _name, _groupName);
	selectionProperty->resetOptions(_values);
}

void PropertyHelper::setBoolPropertyValue(bool _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesBoolean* booleanProeprty = getBoolProperty(_base, _name, _groupName);
	booleanProeprty->setValue(_value);
}

void PropertyHelper::setColourPropertyValue(const ot::Color& _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesColor* colourProperty = getColourProperty(_base, _name, _groupName);

	const double gridColorR = (static_cast<double>(_value.r()) - 0.5) / 255.0;
	const double gridColorG = (static_cast<double>(_value.g()) - 0.5) / 255.0;
	const double gridColorB = (static_cast<double>(_value.b()) - 0.5) / 255.0;
	const double gridColorA = (static_cast<double>(_value.a()) - 0.5) / 255.0;

	colourProperty->setColorRGB(gridColorR, gridColorG, gridColorB);
}

void PropertyHelper::setPainterPropertyValue(const ot::Painter2D* _painter, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesGuiPainter* painterProperty = getPainterProperty(_base, _name, _groupName);
	painterProperty->setValue(_painter);
}

void PropertyHelper::setIntegerPropertyValue(int32_t _value, EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	EntityPropertiesInteger* intProperty = getIntegerProperty(_base, _name, _groupName);
	intProperty->setValue(_value);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Writable property access

EntityPropertiesDouble* PropertyHelper::getDoubleProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	return getProperty<EntityPropertiesDouble>(_base, _name, _groupName);
}

EntityPropertiesString* PropertyHelper::getStringProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	return getProperty<EntityPropertiesString>(_base, _name, _groupName);
}

EntityPropertiesSelection* PropertyHelper::getSelectionProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	return getProperty<EntityPropertiesSelection>(_base, _name, _groupName);
}

EntityPropertiesBoolean* PropertyHelper::getBoolProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	return getProperty<EntityPropertiesBoolean>(_base, _name, _groupName);
}

EntityPropertiesColor* PropertyHelper::getColourProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	return getProperty<EntityPropertiesColor>(_base, _name, _groupName);
}

EntityPropertiesInteger* PropertyHelper::getIntegerProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	return getProperty<EntityPropertiesInteger>(_base, _name, _groupName);
}

EntityPropertiesGuiPainter* PropertyHelper::getPainterProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	return getProperty<EntityPropertiesGuiPainter>(_base, _name, _groupName);
}

EntityPropertiesEntityList* PropertyHelper::getEntityListProperty(EntityBase* _base, const std::string& _name, const std::string& _groupName)
{
	return getProperty<EntityPropertiesEntityList>(_base, _name, _groupName);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Read-only property access

const EntityPropertiesDouble* PropertyHelper::getDoubleProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	return getProperty<EntityPropertiesDouble>(_base, _name, _groupName);
}

const EntityPropertiesString* PropertyHelper::getStringProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	return getProperty<EntityPropertiesString>(_base, _name, _groupName);
}

const EntityPropertiesSelection* PropertyHelper::getSelectionProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	return getProperty<EntityPropertiesSelection>(_base, _name, _groupName);
}

const EntityPropertiesBoolean* PropertyHelper::getBoolProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	return getProperty<EntityPropertiesBoolean>(_base, _name, _groupName);
}

const EntityPropertiesColor* PropertyHelper::getColourProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	return getProperty<EntityPropertiesColor>(_base, _name, _groupName);
}

const EntityPropertiesInteger* PropertyHelper::getIntegerProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	return getProperty<EntityPropertiesInteger>(_base, _name, _groupName);
}

const EntityPropertiesGuiPainter* PropertyHelper::getPainterProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	return getProperty<EntityPropertiesGuiPainter>(_base, _name, _groupName);
}

const EntityPropertiesEntityList* PropertyHelper::getEntityListProperty(const EntityBase* _base, const std::string& _name, const std::string& _groupName) {
	return getProperty<EntityPropertiesEntityList>(_base, _name, _groupName);
}
