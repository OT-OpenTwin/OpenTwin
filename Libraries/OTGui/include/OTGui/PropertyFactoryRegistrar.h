// @otlicense

//! @file PropertyFactoryRegistrar.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"

// std header
#include <string>

namespace ot {

	template <class T>
	class PropertyFactoryRegistrar {
		OT_DECL_NODEFAULT(PropertyFactoryRegistrar)
		OT_DECL_NOCOPY(PropertyFactoryRegistrar)
		OT_DECL_NOMOVE(PropertyFactoryRegistrar)
	public:
		PropertyFactoryRegistrar(const std::string& _key);
		~PropertyFactoryRegistrar() {};
	};

}

#include "OTGui/PropertyFactoryRegistrar.hpp"