//! @file ManagedPropertyRegistrar.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OT header
#include "OTCore/OTAssert.h"
#include "OTGui/Property.h"

// std header
#include <string>

namespace ot {

	class Property;
	class ManagedPropertyObject;

	//! @class ManagedPropertyRegistrar
	//! @brief The ManagedPropertyRegistrar is used to register a property at a objects PropertyManager upon creation.
	//! @tparam T Property type.
	template <class T> class ManagedPropertyRegistrar {
		OT_DECL_NOCOPY(ManagedPropertyRegistrar)
			OT_DECL_NOMOVE(ManagedPropertyRegistrar)
			OT_DECL_NODEFAULT(ManagedPropertyRegistrar)
	public:

		//! @brief Constructor.
		//! Will register the provided property at the given objects PropertyManager under the given group and set property name.
		//! @param _object Object to register at.
		//! @param _groupName Name of the group to place property at.
		//! @param _property Property to register.
		ManagedPropertyRegistrar(ManagedPropertyObject* _object, const std::string& _groupName, ot::Property* _property);
	};


}

template <class T> ot::ManagedPropertyRegistrar<T>::ManagedPropertyRegistrar(ManagedPropertyObject* _object, const std::string& _groupName, ot::Property* _property) {
	OTAssertNullptr(_object);
	OTAssertNullptr(dynamic_cast<T*>(_property));
	OTAssertNullptr(_object->getPropertyManager());
	_object->getPropertyManager()->addProperty(_groupName, _property);
}
