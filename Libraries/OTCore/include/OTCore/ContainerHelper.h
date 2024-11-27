//! @file ContainerHelper.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <map>
#include <list>

namespace ot {

	//! @brief Creates a list containing all the keys from the provided map.
	//! @tparam K Map key type.
	//! @tparam V Map value type.
	//! @param _map Data.
	//! @return List of values.
	template <typename K, typename V> std::list<K> listFromMapKeys(const std::map<K, V>& _map);

	//! @brief Creates a list containing all the values from the provided map.
	//! @tparam K Map key type.
	//! @tparam V Map value type.
	//! @param _map Data.
	//! @return List of values.
	template <typename K, typename V> std::list<V> listFromMapValues(const std::map<K, V>& _map);

	//! @brief Removes the value from the provided map.
	//! @tparam K Map key type.
	//! @tparam V Map value type.
	//! @param _map 
	//! @param _value 
	template <class K, class V> void removeFromMapByValue(std::map<K, V>& _map, const V& _value);

	template <typename T> std::vector<T> vectorFromList(const std::list<T>& _list);
	template <typename T> std::list<T> listFromVector(const std::vector<T>& _vector);
}

template <typename K, typename V>
std::list<K> ot::listFromMapKeys(const std::map<K, V>& _map) {
	std::list<K> result;

	for (const auto& it : _map) {
		result.push_back(it.first);
	}

	return std::move(result);
}

template <typename K, typename V>
std::list<V> ot::listFromMapValues(const std::map<K, V>& _map) {
	std::list<V> result;

	for (const auto& it : _map) {
		result.push_back(it.second);
	}

	return std::move(result);
}

template <class K, class V>
void ot::removeFromMapByValue(std::map<K, V>& _map, const V& _value) {
	for (auto it = _map.begin(); it != _map.end(); ) {
		if (it->second == _value) {
			it = _map.erase(it);
		}
		else {
			it++;
		}
	}
}

template <typename T>
std::vector<T> ot::vectorFromList(const std::list<T>& _list) {
	std::vector<T> result;
	result.reserve(_list.size());
	for (const T& entry : _list) {
		result.push_back(entry);
	}
	return result;
}

template <typename T>
std::list<T> ot::listFromVector(const std::vector<T>& _vector) {
	std::list<T> result;
	for (const T& entry : _vector) {
		result.push_back(entry);
	}
	return result;
}