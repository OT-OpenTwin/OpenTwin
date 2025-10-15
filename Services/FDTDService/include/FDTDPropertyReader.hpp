/*
 * FDTDPropertyReader.hpp
 *
 *  Created on: 15.10.2025
 *	Author: Alexandros McCray (alexm-dev)
 *  Copyright (c) 2025
 */

#pragma once

// OpenTwin
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCore/LogDispatcher.h"
#include "EntityInformation.h"

// STD
#include <type_traits>	// is_integral_v()
#include <string>
#include <stdexcept>
#include <cstdint>
#include <array>
#include <vector>
#include <map>

// Forward declaration
class EntityBase;

 //@brief Template function to read entity properties of different types
 //@param T The type of the property to read (double, int, string, etc.)
 //@param solverEntity Pointer to the solver entity
 //@param groupFilter The group filter to apply when reading properties
 //@param properties Map to store the read properties
 //@param propertyName The name of the property to read
template<typename T>
T readEntityPropertiesInfo(EntityBase* solverEntity, const std::string& groupFilter, std::map<ot::UID, EntityProperties>& properties, const std::string& propertyName) {
	ot::ModelServiceAPI::getEntityProperties(solverEntity->getName(), true, groupFilter, properties);
	for (auto& item : properties) {
		EntityProperties& props = item.second;
		if constexpr (std::is_same<T, double>::value) {
			EntityPropertiesDouble* doubleProp = dynamic_cast<EntityPropertiesDouble*>(props.getProperty(propertyName));
			if (doubleProp != nullptr) {
				return doubleProp->getValue();
			}
		}
		else if constexpr (std::is_integral_v<T>) {
			EntityPropertiesInteger* intProp = dynamic_cast<EntityPropertiesInteger*>(props.getProperty(propertyName));
			if (intProp != nullptr) {
				return static_cast<T>(intProp->getValue());
			}
		}
		else if constexpr (std::is_same<T, std::string>::value) {
			EntityPropertiesSelection* selectionProp = dynamic_cast<EntityPropertiesSelection*>(props.getProperty(propertyName));
			if (selectionProp != nullptr) {
				return selectionProp->getValue();
			}
		}
	}
	OT_LOG_EA("Property not found!");
	if constexpr (std::is_same<T, std::string>::value) return {};
	else return static_cast<T>(0);
}
