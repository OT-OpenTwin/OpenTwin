//! @file PropertyFactoryRegistrar.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// std header
#include <string>

namespace ot {

	template <class T>
	class PropertyFactoryRegistrar {
		OT_DECL_NODEFAULT(PropertyFactoryRegistrar)
		OT_DECL_NOCOPY(PropertyFactoryRegistrar)
	public:
		PropertyFactoryRegistrar(const std::string& _key);
		~PropertyFactoryRegistrar() {};
	};

}

#include "OTCore/PropertyFactoryRegistrar.hpp"