// @otlicense
// File: PropertyStringList.cpp
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

// OpenTwin header
#include "OTCore/JSON/JSON.h"
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/PropertyStringList.h"
#include "OTGui/Properties/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyStringList> propertyStringListRegistrar(ot::PropertyStringList::propertyTypeString());

ot::PropertyStringList::PropertyStringList(const PropertyStringList* _other) 
	: Property(_other), m_current(_other->m_current), m_options(_other->m_options)
{}

ot::PropertyStringList::PropertyStringList(const PropertyBase & _base)
	: Property(_base)
{}

ot::PropertyStringList::PropertyStringList(PropertyFlags _flags)
	: Property(_flags) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _current, PropertyFlags _flags)
	: Property(_flags), m_current(_current) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _current, const std::list<std::string>& _options, PropertyFlags _flags)
	: Property(_flags), m_current(_current)
{
	this->setOptions(_options);
}

ot::PropertyStringList::PropertyStringList(const std::string& _current, const std::vector<std::string>& _options, PropertyFlags _flags)
	: Property(_flags), m_current(_current)
{
	this->setOptions(_options);
}

ot::PropertyStringList::PropertyStringList(const std::string& _name, const std::string& _current, const std::list<std::string>& _options, PropertyFlags _flags)
	: Property(_name, _flags), m_current(_current)
{
	this->setOptions(_options);
}

ot::PropertyStringList::PropertyStringList(const std::string& _name, const std::string& _current, const std::vector<std::string>& _options, PropertyFlags _flags)
	: Property(_name, _flags), m_current(_current)
{
	this->setOptions(_options);
}

void ot::PropertyStringList::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	const PropertyStringList* other = dynamic_cast<const PropertyStringList*>(_other);
	OTAssertNullptr(other);

	if (_mergeMode & PropertyBase::MergeValues) {	
		m_current = other->m_current;
	}
	if (_mergeMode & PropertyBase::MergeConfig) {
		m_options = other->m_options;
	}
}

ot::Property* ot::PropertyStringList::createCopy() const {
	return new PropertyStringList(this);
}

void ot::PropertyStringList::addOption(const std::string& _value, ValueHandlingType _type)
{
	m_options.push_back(std::make_pair(_value, _type));
}

void ot::PropertyStringList::setOptions(const std::list<std::string>& _options) {
	m_options.clear();
	m_options.reserve(_options.size());
	for (const std::string& s : _options) {
		m_options.push_back(std::make_pair(s, ValueHandlingType::Value));
	}
}

void ot::PropertyStringList::setOptions(const std::vector<std::string>& _options) {
	m_options.clear();
	m_options.reserve(_options.size());
	for (const std::string& s : _options) {
		m_options.push_back(std::make_pair(s, ValueHandlingType::Value));
	}
}

void ot::PropertyStringList::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonArray optionsArr;
	for (const auto& opt : m_options) {
		JsonObject optObj;
		optObj.AddMember("V", JsonString(opt.first, _allocator), _allocator);
		optObj.AddMember("T", JsonString(toString(opt.second), _allocator), _allocator);
		optionsArr.PushBack(optObj, _allocator);
	}
	_object.AddMember("Options", optionsArr, _allocator);
	_object.AddMember("Current", JsonString(m_current, _allocator), _allocator);
}

void ot::PropertyStringList::setPropertyData(const ot::ConstJsonObject& _object) {
	if (_object.HasMember("Options"))
	{
		const auto optionsArr = json::getObjectVector(_object, "Options");
		m_options.clear();
		m_options.reserve(optionsArr.size());

		for (const ConstJsonObject& optObj : optionsArr)
		{
			std::string value = json::getString(optObj, "V");
			ValueHandlingType type = stringToValueHandlingType(json::getString(optObj, "T"));
			m_options.push_back(std::make_pair(value, type));
		}
	}
	else
	{
		OTAssert(_object.HasMember("List"), "Invalid");
		for (const std::string& optStr : json::getStringVector(_object, "List"))
		{
			m_options.push_back(std::make_pair(optStr, ValueHandlingType::Value));
		}
	}
	m_current = json::getString(_object, "Current");
}