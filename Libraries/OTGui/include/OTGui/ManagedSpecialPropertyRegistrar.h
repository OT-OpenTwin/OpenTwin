// @otlicense

//! @file ManagedSpecialPropertyRegistrar.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OT header
#include "OTSystem/OTAssert.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyManager.h"
#include "OTGui/ManagedPropertyObject.h"

// std header
#include <string>
#include <optional>
#include <functional>

namespace ot {
	
	class Property;
	class ManagedPropertyObject;

	//! @class ManagedSpecialPropertyRegistrar
	//! @brief The ManagedSpecialPropertyRegistrar is used to register a special property at a objects PropertyManager upon creation.
	class ManagedSpecialPropertyRegistrar {
	public:
		// Object, GroupName, PropertyName
		using CallbackType = std::function<void(ManagedPropertyObject*, const std::string&, const std::string&)>;
		ManagedSpecialPropertyRegistrar(ot::ManagedPropertyObject* _object, const std::string& _groupName, const std::string& _propertyName, CallbackType _initializeMethod) {
			_initializeMethod(_object, _groupName, _propertyName);
		}

	};

}
