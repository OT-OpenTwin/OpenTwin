// @otlicense

//! @file ManagedPropertyObject.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OT header
#include "OTCore/JSON.h"
#include "OTGui/PropertyManagerNotifier.h"

namespace ot {

	class PropertyManager;

	//! @class ManagedPropertyObject
	//! @brief The ManagedPropertyObject class is a basic wrapper for the use of the PropertyManager.
	//! A PropertyManager instance is accessable trough a regular and a const getter methods.
	class OT_GUI_API_EXPORT ManagedPropertyObject : public PropertyManagerNotifier {
		OT_DECL_NOCOPY(ManagedPropertyObject)
	public:
		//! @brief Constructor.
		//! @param _propertyManager Property manager to use. May be null.
		ManagedPropertyObject(PropertyManager* _propertyManager = (PropertyManager*)nullptr);

		//! @brief Move constructor.
		//! @param _other Other object to move data from.
		ManagedPropertyObject(ManagedPropertyObject&& _other) noexcept;

		//! @brief Destructor.
		virtual ~ManagedPropertyObject() {};

		//! @brief Assignment move operator.
		//! @param _other Other object to move data from.
		//! @return Reference to this object.
		ManagedPropertyObject& operator = (ManagedPropertyObject&& _other) noexcept;

		//! @brief Serialize the properties into the provided JSON object.
		//! @param _object JSON object to write the properties into.
		//! @param _allocator JSON allocator to use.
		void addPropertiesToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

		//! @brief Serialize the properties into the provided JSON object.
		//! @param _object JSON object to read the properties from.
		void setPropertiesFromJsonObject(const ot::ConstJsonObject& _object);

		//! @brief Replace the current property manager.
		//! @param _manager Manager to set.
		void setPropertyManager(PropertyManager* _manager) { m_manager = _manager; };

		//! @brief Get the property manager that holds all properties.
		PropertyManager* getPropertyManager(void) const { return m_manager; };

		//! @brief Finds a property by group and name.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return Pointer to the found Property, or nullptr if not found.
		Property* findProperty(const std::string& _groupName, const std::string& _valueName);

		//! @brief Finds a property by group and name.
		//! @param _groupName The property group name.
		//! @param _valueName The property value name.
		//! @return Pointer to the found Property, or nullptr if not found.
		const Property* findProperty(const std::string& _groupName, const std::string& _valueName) const;

	private:
		PropertyManager* m_manager; //! @brief Property manager.

	};

}
