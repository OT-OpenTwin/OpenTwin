// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/PropertyGroup.h"

// std header
#include <map>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! @class PropertyManagerIterator
    //! @brief Iterator to traverse all Property objects in a map of PropertyGroup.
	class OT_GUI_API_EXPORT PropertyManagerIterator {
	public:
        //! @brief Constructs an iterator for a given data map.
        //! @param data Reference to the map containing PropertyGroups.
        PropertyManagerIterator(const std::map<std::string, PropertyGroup*>& _data)
            : m_data(_data), m_listIt(_data.begin()) {
            if (m_listIt != m_data.end()) {
                m_itemIt = m_listIt->second->getProperties().begin();
                moveToValid();
            }
        }

        //! @brief Moves the iterator to the next property.
        //! @return Reference to the updated iterator.
        PropertyManagerIterator& operator++() {
            if (m_listIt == m_data.end()) return *this;

            ++m_itemIt;
            moveToValid();
            return *this;
        }

        //! @brief Checks if two iterators are equal.
        //! @param _other The other iterator to compare against.
        //! @return True if they are equal, false otherwise.
        bool operator==(const PropertyManagerIterator& _other) const {
            return m_listIt == _other.m_listIt && (m_listIt == m_data.end() || m_itemIt == _other.m_itemIt);
        }

        //! @brief Checks if two iterators are not equal.
        //! @param _other The other iterator to compare against.
        //! @return True if they are not equal, false otherwise.
        bool operator!=(const PropertyManagerIterator& _other) const { return !(*this == _other); }

        //! @brief Dereferences the iterator to get the current property.
        //! @return A pointer to the current property.
        Property* operator*() const { return *m_itemIt; }

        //! @brief Returns an iterator to the end of the collection.
        //! @return An iterator pointing past the last valid element.
        static PropertyManagerIterator end(const std::map<std::string, PropertyGroup*>& _data) {
            return PropertyManagerIterator(_data, true);
        }

    private:
        std::map<std::string, PropertyGroup*> m_data; //!< The map containing PropertyGroups
        std::map<std::string, PropertyGroup*>::const_iterator m_listIt; //!< Iterator over the map
        std::list<Property*>::const_iterator m_itemIt; //!< Iterator over the current list

        //! @brief Constructs an end iterator.
        //! @param _data Reference to the data map.
        //! @param _isEnd Indicates if this is an end iterator.
        PropertyManagerIterator(const std::map<std::string, PropertyGroup*>& _data, bool _isEnd)
            : m_data(_data), m_listIt(_data.end()) {
            if (!_isEnd) {
                m_listIt = m_data.begin();
                if (m_listIt != m_data.end()) {
                    m_itemIt = m_listIt->second->getProperties().begin();
                    moveToValid();
                }
            }
        }

        //! @brief Moves the iterator to a valid position.
        void moveToValid() {
            while (m_listIt != m_data.end() && m_itemIt == m_listIt->second->getProperties().end()) {
                ++m_listIt;
                if (m_listIt != m_data.end()) {
                    m_itemIt = m_listIt->second->getProperties().begin();
                }
            }
        }
	};

}