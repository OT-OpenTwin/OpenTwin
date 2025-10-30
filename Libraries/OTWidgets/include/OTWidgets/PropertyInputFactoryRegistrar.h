// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// std header
#include <string>

namespace ot {

	template <class T>
	class PropertyInputFactoryRegistrar {
		OT_DECL_NODEFAULT(PropertyInputFactoryRegistrar)
		OT_DECL_NOCOPY(PropertyInputFactoryRegistrar)
		OT_DECL_NOMOVE(PropertyInputFactoryRegistrar)
	public:
		PropertyInputFactoryRegistrar(const std::string& _key);
		~PropertyInputFactoryRegistrar() {};
	};

}

#include "OTWidgets/PropertyInputFactoryRegistrar.hpp"