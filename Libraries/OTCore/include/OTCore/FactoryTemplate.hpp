//! @file FactoryTemplate.hpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/Serializable.h"
#include "OTCore/FactoryTemplate.h"

template<class KeyType, class ValueType>
void ot::FactoryTemplate<KeyType, ValueType>::registerConstructor(const KeyType& _key, ConstructorFunctionType _constructor) {
	auto it = m_constructors.find(_key);
	if (it != m_constructors.end()) {
		OT_LOG_E("Constructor already registered");
		return;
	}
}

template<class KeyType, class ValueType>
void ot::FactoryTemplate<KeyType, ValueType>::deregisterConstructor(const KeyType& _key) {
	m_constructors.erase(_key);
}

template<class KeyType, class ValueType>
std::shared_ptr<ValueType> ot::FactoryTemplate<KeyType, ValueType>::create(const KeyType& _key) {
	auto it = m_constructors.find(_key);
	if (it == m_constructors.end()) {
		OT_LOG_E("Constructor not found");
		return nullptr;
	}
	else {
		return it->second();
	}	
}

template<class KeyType, class ValueType>
std::shared_ptr<ValueType> ot::FactoryTemplate<KeyType, ValueType>::create(const ConstJsonObject& _jsonObject, const std::string& _typeKey) {
	std::string t = json::getString(_jsonObject, _typeKey.c_str());
	if (t.empty()) {
		OT_LOG_E("Key empty");
		return nullptr;
	}

	std::shared_ptr<ValueType> ret = this->create(t);

	// Check if the created object is a serializable object
	Serializable* s = dynamic_cast<Serializable*>(ret.get());
	if (s) {
		s->setFromJsonObject(_jsonObject);
	}

	return ret;
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
		return std::make_shared<CreatedType>();
		});
}

template <typename KeyType, class FactoryType, typename CreatedType>
ot::FactoryRegistrarTemplate<KeyType, FactoryType, CreatedType>::~FactoryRegistrarTemplate() {
	FactoryType::instance().deregisterConstructor(m_key);
}