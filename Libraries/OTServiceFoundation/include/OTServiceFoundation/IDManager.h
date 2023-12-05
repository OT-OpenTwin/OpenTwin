/*
 *  IDManager.h
 *
 *  Created on: 23/09/2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022, OpenTwin
 */

#pragma once

// OpenTwin header
#include "OTCore/OTAssert.h"

// C++ header
#include <map>

namespace ot {

	template<class T> class IDManager {
	public:
		IDManager();
		virtual ~IDManager();

		//! @brief Will find the next free id and return it
		T grabNextID(void);

		//! @brief Will set the provided id as free
		void freeID(T _id);

	private:
		T					m_currentId;
		std::map<T, bool>	m_occupiedIDs;
	};

	template <class T> IDManager<T>::IDManager() : m_currentId(0) {}
	template <class T> IDManager<T>::~IDManager() {}

	template <class T> T IDManager<T>::grabNextID(void) {
		T id = ++m_currentId;
		bool end = false;
		while (m_occupiedIDs.find(id) != m_occupiedIDs.end() || id == 0) {
			if (id == 0) {
				if (end) {
					OTAssert(0, "There is no free ID available");
					return 0;
				}
				else {
					end = true;
				}
			}
			id++;
		}
		return id;
	}

	template <class T> void IDManager<T>::freeID(T _id) { m_occupiedIDs.erase(_id); }
}