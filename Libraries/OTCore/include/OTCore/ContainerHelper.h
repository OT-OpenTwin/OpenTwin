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
		template <typename K, typename V> static std::list<K> listFromMapKeys(const std::map<K, V>& _map);

		//! @brief Creates a list containing all the values from the provided map.
		//! @tparam K Map key type.
		//! @tparam V Map value type.
		//! @param _map Data.
		//! @return List of values.
		template <typename K, typename V> static std::list<V> listFromMapValues(const std::map<K, V>& _map);

		//! @brief Removes the value from the provided map.
		//! @tparam K Map key type.
		//! @tparam V Map value type.
		//! @param _map 
		//! @param _value 
		template <class K, class V> static void removeFromMapByValue(std::map<K, V>& _map, const V& _value);

		//! @brief Returns true if the provided map stores an entry with the given value.
		//! @tparam K Map key type.
		//! @tparam V Map value type.
		//! @param _map 
		//! @param _value 
		template <class K, class V> static bool hasValue(const std::map<K, V>& _map, const V& _value);

		template <typename T> static std::vector<T> vectorFromList(const std::list<T>& _list);
		template <typename T> static std::list<T> listFromVector(const std::vector<T>& _vector);

		template <typename T> static std::list<T> createDiff(const std::list<T>& _left, const std::list<T>& _right, DiffMode _diffMode);
		template <typename T> static std::vector<T> createDiff(const std::vector<T>& _left, const std::vector<T>& _right, DiffMode _diffMode);

	};	
}

#include "OTCore/ContainerHelper.hpp"
