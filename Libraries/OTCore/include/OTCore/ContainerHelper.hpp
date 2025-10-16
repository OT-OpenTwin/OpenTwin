//! @file ContainerHelper.hpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/ContainerHelper.h"

// std header
#include <set>
#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

template <typename K, typename V>
inline std::list<K> ot::ContainerHelper::getKeys(const std::map<K, V>& _map) {
	std::list<K> result;

	for (const auto& it : _map) {
		result.push_back(it.first);
	}

	return result;
}

template <typename K, typename V>
inline std::list<V> ot::ContainerHelper::getValues(const std::map<K, V>& _map) {
	std::list<V> result;

	for (const auto& it : _map) {
		result.push_back(it.second);
	}

	return result;
}

template <class K, class V>
inline void ot::ContainerHelper::removeByValue(std::map<K, V>& _map, const V& _value) {
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
inline bool ot::ContainerHelper::hasValue(const std::map<K, V>& _map, const V& _value) {
	for (auto it = _map.begin(); it != _map.end(); it++) {
		if (it->second == _value) {
			return true;
		}
	}
	return false;
}

template<class K, class V>
inline bool ot::ContainerHelper::contains(const std::map<K, V>& _map, const K& _key) {
	return _map.find(_key) != _map.end();
}

template<typename T>
inline bool ot::ContainerHelper::contains(const std::list<T>& _list, const T& _value) {
	return std::find(_list.begin(), _list.end(), _value) != _list.end();
}

template<typename T>
inline bool ot::ContainerHelper::contains(const std::vector<T>& _vector, const T& _value) {
	return std::find(_vector.begin(), _vector.end(), _value) != _vector.end();
}

template <typename T>
inline std::vector<T> ot::ContainerHelper::toVector(const std::list<T>& _list) {
	std::vector<T> result;
	result.reserve(_list.size());
	for (const T& entry : _list) {
		result.push_back(entry);
	}
	return result;
}

template <typename T>
inline std::list<T> ot::ContainerHelper::toList(const std::vector<T>& _vector) {
	std::list<T> result;
	for (const T& entry : _vector) {
		result.push_back(entry);
	}
	return result;
}

template<typename T>
inline std::list<T> ot::ContainerHelper::createDiff(const std::list<T>& _left, const std::list<T>& _right, DiffMode _diffMode) {
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
inline std::vector<T> ot::ContainerHelper::createDiff(const std::vector<T>& _left, const std::vector<T>& _right, DiffMode _diffMode) {
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

template<typename T>
inline bool ot::ContainerHelper::isSubset(const std::list<T>& _subset, const std::list<T>& _list) {
	std::unordered_set<T> data(_list.begin(), _list.end());

	for (const T& val : _subset) {
		if (data.find(val) == data.end()) {
			return false; // value is not in list
		}
	}

	return true;
}

template<typename T>
inline bool ot::ContainerHelper::isSubset(const std::vector<T>& _subset, const std::vector<T>& _vector) {
	std::unordered_set<T> data(_vector.begin(), _vector.end());

	for (const T& val : _subset) {
		if (data.find(val) == data.end()) {
			return false; // value is not in vector
		}
	}

	return true;
}

template<typename T>
inline std::list<T> ot::ContainerHelper::intersect(const std::list<T>& _list1, const std::list<T>& _list2) {
	std::list<T> result;
	for (const auto& item : _list1) {
		if (std::find(_list2.begin(), _list2.end(), item) != _list2.end()) {
			result.push_back(item);
		}
	}
	return result;
}

template<typename T>
inline std::vector<T> ot::ContainerHelper::intersect(const std::vector<T>& _vector1, const std::vector<T>& _vector2) {
	std::vector<T> result;
	for (const auto& item : _vector1) {
		if (std::find(_vector2.begin(), _vector2.end(), item) != _vector2.end()) {
			result.push_back(item);
		}
	}
	return result;
}

template<typename T>
inline bool ot::ContainerHelper::hasIntersection(const std::list<T>& _list1, const std::list<T>& _list2) {
	for (const auto& item : _list1) {
		if (std::find(_list2.begin(), _list2.end(), item) != _list2.end()) {
			return true;
		}
	}
	return false;
}

template<typename T>
inline bool ot::ContainerHelper::hasIntersection(const std::vector<T>& _vector1, const std::vector<T>& _vector2) {
	std::unordered_set<T> set1(_vector1.begin(), _vector1.end());
	for (const auto& item : _vector2) {
		if (set1.find(item) != set1.end()) {
			return true;
		}
	}
	return false;
}

namespace ot {
	namespace intern {
		// Helper: compare contents via hashing (O(n))
		template <typename T>
		static inline bool isEqualHashing(const T& _c1, const T& _c2) {
			if (_c1.size() != _c2.size()) {
				return false;
			}

			std::unordered_map<typename T::value_type, size_t> counts;
			for (const auto& item : _c1) {
				++counts[item];
			}

			for (const auto& item : _c2) {
				auto it = counts.find(item);
				if (it == counts.end() || it->second == 0) {
					return false;
				}
				--it->second;
			}

			return true;
		}

		// Helper: compare contents via sorting (O(n log n))
		template <typename T>
		static inline bool isEqualSorting(const T& _c1, const T& _c2) {
			if (_c1.size() != _c2.size()) {
				return false;
			}

			std::vector<typename T::value_type> v1(_c1.begin(), _c1.end());
			std::vector<typename T::value_type> v2(_c2.begin(), _c2.end());

			std::sort(v1.begin(), v1.end());
			std::sort(v2.begin(), v2.end());

			return v1 == v2;
		}
	}
}

template <typename T>
inline bool ot::ContainerHelper::isEqual(const std::list<T>& _list1, const std::list<T>& _list2) {
	if constexpr (std::is_default_constructible_v<std::hash<T>>) {
		// Prefer hashing if available
		return intern::isEqualHashing(_list1, _list2);
	}
	else {
		return intern::isEqualSorting(_list1, _list2);
	}
}

template <typename T>
inline bool ot::ContainerHelper::isEqual(const std::vector<T>& _vector1, const std::vector<T>& _vector2) {
	if constexpr (std::is_default_constructible_v<std::hash<T>>) {
		return intern::isEqualHashing(_vector1, _vector2);
	}
	else {
		return intern::isEqualSorting(_vector1, _vector2);
	}
}