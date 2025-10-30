// @otlicense
// File: FactoryTemplate.hpp
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

#pragma once

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/Serializable.h"
#include "OTCore/FactoryTemplate.h"

// std header
#include <type_traits>

template<class ValueType>
void ot::FactoryTemplate<ValueType>::registerConstructor(const std::string& _key, ConstructorFunctionType _constructor) {
	auto it = m_constructors.find(_key);
	if (it != m_constructors.end()) {
		OT_LOG_E("Constructor already registered");
		return;
	}
	m_constructors.insert_or_assign(_key, _constructor);
}

template<class ValueType>
void ot::FactoryTemplate<ValueType>::deregisterConstructor(const std::string& _key) {
	m_constructors.erase(_key);
}

template<class ValueType>
ValueType* ot::FactoryTemplate<ValueType>::createFromKey(const std::string& _key) {
	auto it = m_constructors.find(_key);
	if (it == m_constructors.end()) {
		OT_LOG_E("Constructor \"" + _key + "\" not found");
		return nullptr;
	}
	else {
		return it->second();
	}	
}

template<class ValueType> 
ValueType* ot::FactoryTemplate<ValueType>::createFromJSON(const ConstJsonObject& _jsonObject, const char* _typeKey) {
	std::string t = json::getString(_jsonObject, _typeKey);
	if (t.empty()) {
		return nullptr;
	}

	ValueType* ret = this->createFromKey(t);

	// Check if the created object is a serializable object
	Serializable* s = dynamic_cast<Serializable*>(ret);
	if (s) {
		s->setFromJsonObject(_jsonObject);
	}

	return ret;
}

template<class ValueType>
ValueType* ot::FactoryTemplate<ValueType>::createFromJSON(const ConstJsonObject& _jsonObject, const std::string& _typeKey) {
	return this->createFromJSON(_jsonObject, _typeKey.c_str());
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

template <class FactoryType, typename CreatedType>
ot::FactoryRegistrarTemplate<FactoryType, CreatedType>::FactoryRegistrarTemplate(const std::string& _key)
	: m_key(_key)
{
	static_assert(std::is_same<decltype(&FactoryType::instance), decltype(&FactoryType::instance)>::value,
		"Factory must have a static 'instance' function.");

	static_assert(std::is_same<decltype(&FactoryType::instance), FactoryType& (*)()>::value,
		"Factory must have a static 'instance' function which returns a reference to the global factory instance.");

	FactoryType::instance().registerConstructor(_key, []() {
		return new CreatedType;
		});
}

template <class FactoryType, typename CreatedType>
ot::FactoryRegistrarTemplate<FactoryType, CreatedType>::~FactoryRegistrarTemplate() {
	FactoryType::instance().deregisterConstructor(m_key);
}