//! @file IDManager.h
//! @authors Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTSystem/Exception.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <unordered_set>

namespace ot {

	//! @brief The IDManager class is used to generate IDs.
	//! The generated IDs will be stored.
	//! @tparam T ID type.
	template<class T> class IDManager {
		OT_DECL_NOCOPY(IDManager)
		OT_DECL_DEFMOVE(IDManager)
	public:
		//! @brief Constructor.
		IDManager();

		//! @brief Destructor.
		~IDManager();

		//! @brief Find and return the next free id.
		T nextID();

		//! @brief Remove the provided ID from the list of occupied IDs.
		void freeID(T _id);

	private:
		T                     m_currentId;
		std::unordered_set<T> m_occupiedIDs;
	};
}

// ###########################################################################################################################################################################################################################################################################################################################

template <class T> ot::IDManager<T>::IDManager() :
	m_currentId(static_cast<T>(0)) {
}

template <class T> ot::IDManager<T>::~IDManager() {}

template <class T> T ot::IDManager<T>::nextID(void) {
	T id = ++m_currentId;

	// end is used to determine if the loop has already reached the end of the IDs
	bool end = false;

	while (m_occupiedIDs.find(id) != m_occupiedIDs.end() || id == static_cast<T>(0)) {
		if (id == 0) {
			if (end) {
				throw ot::Exception::OutOfBounds("No free ID available");
			}
			else {
				end = true;
			}
		}
		id++;
	}
	return id;
}

template <class T> void ot::IDManager<T>::freeID(T _id) { m_occupiedIDs.erase(_id); }