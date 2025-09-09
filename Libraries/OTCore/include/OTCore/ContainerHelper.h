//! @file ContainerHelper.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <map>
#include <list>
#include <vector>

namespace ot {

	class ContainerHelper {
	public:
		enum DiffMode {
			MissingLeft, //! @brief Items missing in the left list that exist in the right list.
			MissingRight, //! @brief Items missing in the right list that exist in the left list.
			MissingEither //! @brief Items missing in any of the two lists.
		};

		//! @brief Creates a list containing all the keys from the provided map.
		//! @tparam K Map key type.
		//! @tparam V Map value type.
		//! @param _map Data.
		//! @return List of values.
		template <typename K, typename V> static std::list<K> getKeys(const std::map<K, V>& _map);

		//! @brief Creates a list containing all the values from the provided map.
		//! @tparam K Map key type.
		//! @tparam V Map value type.
		//! @param _map Data.
		//! @return List of values.
		template <typename K, typename V> static std::list<V> getValues(const std::map<K, V>& _map);

		//! @brief Removes the value from the provided map.
		//! @tparam K Map key type.
		//! @tparam V Map value type.
		//! @param _map 
		//! @param _value 
		template <class K, class V> static void removeByValue(std::map<K, V>& _map, const V& _value);

		//! @brief Returns true if the provided map stores an entry with the given value.
		//! @tparam K Map key type.
		//! @tparam V Map value type.
		//! @param _map 
		//! @param _value 
		template <class K, class V> static bool hasValue(const std::map<K, V>& _map, const V& _value);

		template <typename T> static std::vector<T> toVector(const std::list<T>& _list);
		template <typename T> static std::list<T> toList(const std::vector<T>& _vector);

		template <typename T> static std::list<T> createDiff(const std::list<T>& _left, const std::list<T>& _right, DiffMode _diffMode);
		template <typename T> static std::vector<T> createDiff(const std::vector<T>& _left, const std::vector<T>& _right, DiffMode _diffMode);

		//! @brief Returns true if _subset is a subset of _list.
		//! @tparam T List value type.
		//! @param _subset List expected to be a subset.
		//! @param _list List expected to contain the subset.
		template <typename T> static bool isSubset(const std::list<T>& _subset, const std::list<T>& _list);

		//! @brief Returns true if _subset is a subset of _vector.
		//! @tparam T Vector value type.
		//! @param _subset Vector expected to be a subset.
		//! @param _vector Vector expected to contain the subset.
		template <typename T> static bool isSubset(const std::vector<T>& _subset, const std::vector<T>& _vector);

		//! @brief Returns true if _list1 has at least one matching item in _list2.
		//! @tparam T List value type.
		//! @param _list1 List 1.
		//! @param _list2 List 2.
		template <typename T> static bool hasIntersection(const std::list<T>& _list1, const std::list<T>& _list2);

		//! @brief Returns true if _vector1 has at least one matching item in _vector2.
		//! @tparam T Vector value type.
		//! @param _vector1 Vector 1.
		//! @param _vector2 Vector 2.
		template <typename T> static bool hasIntersection(const std::vector<T>& _vector1, const std::vector<T>& _vector2);

		//! @brief Returns true if the provided lists contain the same items (order does not matter).
		//! @tparam T Vector value type.
		//! @param _list1 List 1.
		//! @param _list2 List 2.
		template <typename T> static bool isEqual(const std::list<T>& _list1, const std::list<T>& _list2);

		//! @brief Returns true if the provided vectors contain the same items (order does not matter).
		//! @tparam T Vector value type.
		//! @param _vector1 Vector 1.
		//! @param _vector2 Vector 2.
		template <typename T> static bool isEqual(const std::vector<T>& _vector1, const std::vector<T>& _vector2);

	};	
}

#include "OTCore/ContainerHelper.hpp"
