// @otlicense
// File: ObjectManagerTemplate.h
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

// std header
#include <map>

//#pragma warning(disable:4251)

namespace ot {

	//! @param K Key type
	//! @param V Value/Object type
	template <class K, class V>
	class ObjectManagerTemplate {
	public:
		virtual ~ObjectManagerTemplate() {};

		//! @brief Store the provided object for the given key.
		//! If there exists an entry for the given key the function will terminate.
		//! @param _object Pointer to object to store (Object Manager takes ownership).
		//! @return Returns false if there is an entry for the given key (Object not inserterd).
		bool insert(const K& _key, V* _object);

		//! @brief Store the provided object for the given key.
		//! If there exists an entry for the given key the entry will be replaced.
		//! @param _object Pointer to object to store (Object Manager takes ownership).
		void replace(const K& _key, V* _object);

		//! @brief Returns true if an entry exits for the given key.
		//! @param _key The key to check.
		bool contains(const K& _key) { return (bool)this->find(_key); };

		//! @brief Return stored pointer to object for the given key.
		//! Note that the Object Manager keeps the ownership.
		//! @param _key The key to the object.
		//! @return nullptr if no entry found for the specified key.
		V* const find(const K& _key);

		//! @brief Return stored pointer to object for the given key and remove from object manager.
		//! Note that the caller takes ownership.
		//! @param _key The key to the object.
		//! @return nullptr if no entry found for the specified key.
		V* grab(const K& _key);

		//! @brief Return stored pointer to object for the given key.
		//! Note that the Object Manager keeps the ownership.
		//! @param _key The key to the object.
		//! @return nullptr if no entry found for the specified key.
		V* const operator [](const K& _key);

	private:
		std::map<K, V *> m_objects;
	};
}

template <class K, class V>
bool ot::ObjectManagerTemplate<K, V>::insert(const K& _key, V* _object) {
	if (m_objects.find(_key) != m_objects.end()) { return false; }
	else { m_objects.insert_or_assign(_key, _object); return true; }
}

template <class K, class V>
void ot::ObjectManagerTemplate<K, V>::replace(const K& _key, V* _object) {
	if (m_objects.find(_key) != m_objects.end()) { delete m_objects.find(_key)->second; };
	m_objects.insert_or_assign(_key, _object);
}

template <class K, class V>
V* const ot::ObjectManagerTemplate<K, V>::find(const K& _key) {
	auto it = m_objects.find(_key);
	if (it == m_objects.end()) { return (V*)nullptr; }
	else { return it->second; }
}

template <class K, class V>
V* ot::ObjectManagerTemplate<K, V>::grab(const K& _key) {
	auto it = m_objects.find(_key);
	V* ret = (it != m_objects.end() ? it->second : (V*)nullptr);
	m_objects.erase(_key);
	return ret;
}

template <class K, class V>
V* const ot::ObjectManagerTemplate<K, V>::operator [](const K& _key) {
	auto it = m_objects.find(_key);
	if (it == m_objects.end()) { return (V*)nullptr; }
	else { return it->second; }
}