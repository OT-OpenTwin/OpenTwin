//! @file ContainerHelper.hpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/ContainerHelper.h"

// std header
#include <set>
#include <algorithm>

template <typename K, typename V>
std::list<K> ot::ContainerHelper::getKeys(const std::map<K, V>& _map) {
	std::list<K> result;

	for (const auto& it : _map) {
		result.push_back(it.first);
	}

	return result;
}

template <typename K, typename V>
std::list<V> ot::ContainerHelper::getValues(const std::map<K, V>& _map) {
	std::list<V> result;

	for (const auto& it : _map) {
		result.push_back(it.second);
	}

	return result;
}

template <class K, class V>
void ot::ContainerHelper::removeByValue(std::map<K, V>& _map, const V& _value) {
	for (auto it = _map.begin(); it != _map.end(); ) {
		if (it->second == _value) {
			it = _map.erase(it);
		}
		else {
			it++;
		}
	}
}

template<class K, class V>
bool ot::ContainerHelper::hasValue(const std::map<K, V>& _map, const V& _value) {
	for (const auto& it = _map.begin(); it != _map.end(); it++) {
		if (it->second == _value) {
			return true;
		}
	}
	return false;
}

template <typename T>
std::vector<T> ot::ContainerHelper::toVector(const std::list<T>& _list) {
	std::vector<T> result;
	result.reserve(_list.size());
	for (const T& entry : _list) {
		result.push_back(entry);
	}
	return result;
}

template <typename T>
std::list<T> ot::ContainerHelper::toList(const std::vector<T>& _vector) {
	std::list<T> result;
	for (const T& entry : _vector) {
		result.push_back(entry);
	}
	return result;
}

template<typename T>
std::list<T> ot::ContainerHelper::createDiff(const std::list<T>& _left, const std::list<T>& _right, DiffMode _diffMode) {
	std::set<T> leftSet(_left.begin(), _left.end());
	std::set<T> rightSet(_right.begin(), _right.end());
	std::list<T> diff;

	switch (_diffMode) {
	case ot::ContainerHelper::MissingLeft:
		std::set_difference(rightSet.begin(), rightSet.end(), leftSet.begin(), leftSet.end(), std::back_inserter(diff));
		break;

	case ot::ContainerHelper::MissingRight:
		std::set_difference(leftSet.begin(), leftSet.end(), rightSet.begin(), rightSet.end(), std::back_inserter(diff));
		break;

	case ot::ContainerHelper::MissingEither:
		std::set_symmetric_difference(leftSet.begin(), leftSet.end(), rightSet.begin(), rightSet.end(), std::back_inserter(diff));
		break;

	default:
		OT_LOG_E("Unknown diff mode (" + std::to_string(static_cast<int>(_diffMode)) + ")");
		break;
	}

	return diff;
}

template<typename T>
std::vector<T> ot::ContainerHelper::createDiff(const std::vector<T>& _left, const std::vector<T>& _right, DiffMode _diffMode) {
	std::set<T> leftSet(_left.begin(), _left.end());
	std::set<T> rightSet(_right.begin(), _right.end());
	std::vector<T> diff;

	switch (_diffMode) {
	case ot::ContainerHelper::MissingLeft:
		std::set_difference(rightSet.begin(), rightSet.end(), leftSet.begin(), leftSet.end(), std::back_inserter(diff));
		break;

	case ot::ContainerHelper::MissingRight:
		std::set_difference(leftSet.begin(), leftSet.end(), rightSet.begin(), rightSet.end(), std::back_inserter(diff));
		break;

	case ot::ContainerHelper::MissingEither:
		std::set_symmetric_difference(leftSet.begin(), leftSet.end(), rightSet.begin(), rightSet.end(), std::back_inserter(diff));
		break;

	default:
		OT_LOG_E("Unknown diff mode (" + std::to_string(static_cast<int>(_diffMode)) + ")");
		break;
	}

	return diff;
}
