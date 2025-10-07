//! @file PointerList.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// std header
#include <list>

namespace ot {

	//! @brief A simple container to manage pointers of type T.
	//! The container takes ownership of the pointers added to it and will delete them when the container is destroyed.
	//! @tparam T The type of the pointers to be managed.
	template <typename T> class PointerList {
		OT_DECL_NOCOPY(PointerList)
	public:
		//! @brief Default constructor.
		PointerList() = default;

		//! @brief Move constructor.
		//! @param _other The other container to move from.
		PointerList(PointerList&& _other) noexcept;

		//! @brief Destructor.
		//! Deletes all pointers stored in the container.
		~PointerList();

		//! @brief Move assignment operator.
		//! @param _other The other container to move from.
		//! @return A reference to this container.
		PointerList& operator=(PointerList&& _other) noexcept;

		//! @brief Adds a pointer to the container.
		//! The container takes ownership of the pointer and will delete it when the container is destroyed.
		//! @param _item The pointer to be added.
		void add(T* _item);

		//! @brief Removes a pointer from the container.
		//! The caller takes ownership of the pointer.
		//! @param _item The pointer to be removed.
		void remove(T* _item);

		//! @brief Clears the container.
		//! Deletes all pointers stored in the container.
		void clear();

		//! @brief Returns the number of pointers stored in the container.
		size_t size() const { return m_items.size(); };

	private:
		std::list<T*> m_items;
	};

}

#include "OTCore/PointerList.hpp"