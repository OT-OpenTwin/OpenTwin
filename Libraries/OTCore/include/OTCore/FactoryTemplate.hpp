//! @file FactoryTemplate.hpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/Serializable.h"
#include "OTCore/FactoryTemplate.h"

// std header
#include <string>
#include <type_traits>

template<class KeyType, class ValueType>
void ot::FactoryTemplate<KeyType, ValueType>::registerConstructor(const KeyType& _key, ConstructorFunctionType _constructor) {
	auto it = m_constructors.find(_key);
	if (it != m_constructors.end()) {
		OT_LOG_E("Constructor already registered");
		return;
	}
	m_constructors.insert_or_assign(_key, _constructor);
}

template<class KeyType, class ValueType>
void ot::FactoryTemplate<KeyType, ValueType>::deregisterConstructor(const KeyType& _key) {
	m_constructors.erase(_key);
}

template<class KeyType, class ValueType>
ValueType* ot::FactoryTemplate<KeyType, ValueType>::createFromKey(const KeyType& _key) {
	auto it = m_constructors.find(_key);
	if (it == m_constructors.end()) {
		if constexpr (std::is_same<KeyType, std::string>::value) {
			OT_LOG_E("Constructor \"" + _key + "\" not found");
		}
		else if constexpr (std::is_same<KeyType, short>::value) {
			OT_LOG_E("Constructor (" + std::to_string(_key) + ") not found");
		}
		else if constexpr (std::is_same<KeyType, unsigned short>::value) {
			OT_LOG_E("Constructor (" + std::to_string(_key) + ") not found");
		}
		else if constexpr (std::is_same<KeyType, int>::value) {
			OT_LOG_E("Constructor (" + std::to_string(_key) + ") not found");
		}
		else if constexpr (std::is_same<KeyType, unsigned int>::value) {
			OT_LOG_E("Constructor (" + std::to_string(_key) + ") not found");
		}
		else if constexpr (std::is_same<KeyType, long>::value) {
			OT_LOG_E("Constructor (" + std::to_string(_key) + ") not found");
		}
		else if constexpr (std::is_same<KeyType, unsigned long>::value) {
			OT_LOG_E("Constructor (" + std::to_string(_key) + ") not found");
		}
		else if constexpr (std::is_same<KeyType, long long>::value) {
			OT_LOG_E("Constructor (" + std::to_string(_key) + ") not found");
		}
		else if constexpr (std::is_same<KeyType, unsigned long long>::value) {
			OT_LOG_E("Constructor (" + std::to_string(_key) + ") not found");
		}
		else {
			OT_LOG_E("Constructor not found");
		}
		return nullptr;
	}
	else {
		return it->second();
	}	
}

template<class KeyType, class ValueType> 
ValueType* ot::FactoryTemplate<KeyType, ValueType>::createFromJSON(const ConstJsonObject& _jsonObject, const char* _typeKey) {
	std::string t = json::getString(_jsonObject, _typeKey);
	if (t.empty()) {
		OT_LOG_E("Key empty");
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

template<class KeyType, class ValueType>
ValueType* ot::FactoryTemplate<KeyType, ValueType>::createFromJSON(const ConstJsonObject& _jsonObject, const std::string& _typeKey) {
	return this->createFromJSON(_jsonObject, _typeKey.c_str());
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

template <typename KeyType, class FactoryType, typename CreatedType>
ot::FactoryRegistrarTemplate<KeyType, FactoryType, CreatedType>::FactoryRegistrarTemplate(const KeyType& _key)
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

template <typename KeyType, class FactoryType, typename CreatedType>
ot::FactoryRegistrarTemplate<KeyType, FactoryType, CreatedType>::~FactoryRegistrarTemplate() {
	FactoryType::instance().deregisterConstructor(m_key);
}