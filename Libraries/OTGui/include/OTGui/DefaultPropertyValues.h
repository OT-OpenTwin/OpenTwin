//! @file DefaultPropertyValues.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/PropertyManager.h"

// std header
#include <map>
#include <string>

namespace ot {

	//! @brief Manages various property types and serialization.
	//! The PropertyManager provides functions to set, get, and manage different property types.
	//! It also supports serialization to and from JSON objects.
	class OT_GUI_API_EXPORT DefaultPropertyValues : public Serializable {
	public:
		// Constructor

		//! @brief Constructor.
		DefaultPropertyValues();

		//! @brief Deserialize constructor.
		DefaultPropertyValues(const ConstJsonObject& _object);

		//! @brief Move constructor.
		//! @param _other Other object.
		DefaultPropertyValues(DefaultPropertyValues&& _other) noexcept;
		
		//! @brief Destructor.
		virtual ~DefaultPropertyValues();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operator

		//! @brief Move assignment operator.
		//! @param _other Other object.
		DefaultPropertyValues& operator = (DefaultPropertyValues&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods
		
		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Store an objects properties.
		//! @param _objectName Name of the object.
		//! @param _properties 
		void setObjectProperties(const std::string& _objectName, PropertyManager&& _properties);

		//! @brief Get existing object properties.
		//! If properties don't exist a reference to the default PropertyManager will be returned.
		//! @param _objectName Name of the object.
		const PropertyManager& getObjectProperties(const std::string& _objectName) const;

	private:
		//! @brief Default property manager.
		PropertyManager m_defaultManager;

		//! @brief Maps object names to their PropertyManager.
		std::map<std::string, PropertyManager> m_objectToPropertiesMap;
	};

}
