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

#include "FDTDPropertyReader.h"

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

template<typename T>
T readEntityPropertiesInfo(EntityBase* _solverEntity, const std::string& _groupFilter, std::map<ot::UID, EntityProperties>& _properties, const std::string& _propertyName, bool _modelServiceAPICall) {
	if (_modelServiceAPICall) { 
		ot::ModelServiceAPI::getEntityProperties(_solverEntity->getName(), true, _groupFilter, _properties);
	}
	for (auto& item : _properties) {
		EntityProperties& props = item.second;
		if constexpr (std::is_same<T, double>::value) {
			EntityPropertiesDouble* doubleProp = dynamic_cast<EntityPropertiesDouble*>(props.getProperty(_propertyName));
			if (doubleProp != nullptr) {
				return doubleProp->getValue();
			}
		}
		else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
			EntityPropertiesInteger* intProp = dynamic_cast<EntityPropertiesInteger*>(props.getProperty(_propertyName));
			if (intProp != nullptr) {
				return static_cast<T>(intProp->getValue());
			}
		}
		else if constexpr (std::is_same<T, std::string>::value) {
			EntityPropertiesSelection* selectionProp = dynamic_cast<EntityPropertiesSelection*>(props.getProperty(_propertyName));
			if (selectionProp != nullptr) {
				return selectionProp->getValue();
			}
		}
	}
	OT_LOG_EAS("Property '" + _propertyName + "' not found!");
	if constexpr (std::is_same<T, std::string>::value) {
		return {};
	}
	else { 
		return static_cast<T>(0); 
	}
}
