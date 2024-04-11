//! @file PropertyInputFactory.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// std header
#include <map>
#include <string>
#include <functional>

namespace ot {
	
	class Property;
	class PropertyInput;

	class OT_WIDGETS_API_EXPORT PropertyInputFactory {
		OT_DECL_NOCOPY(PropertyInputFactory)
	public:
		using PropertyInputConstructor = std::function<PropertyInput* ()>;

		static PropertyInputFactory& instance(void);
		static PropertyInput* createInput(const std::string& _key);
		static PropertyInput* createInput(const Property* _config);
		static bool registerPropertyInput(const std::string& _key, const PropertyInputConstructor& _constructor);
	private:
		PropertyInputFactory() {};
		~PropertyInputFactory() {};

		std::map<std::string, PropertyInputConstructor> m_constructors;
	};

}