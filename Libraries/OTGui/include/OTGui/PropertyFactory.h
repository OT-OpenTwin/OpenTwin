//! @file PropertyFactory.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTGui/Property.h"

// std header
#include <map>
#include <string>
#include <functional>

namespace ot {
	class Property;

	class OT_GUI_API_EXPORT PropertyFactory {
		OT_DECL_NOCOPY(PropertyFactory)
		OT_DECL_NOMOVE(PropertyFactory)
	public:
		using PropertyConstructor = std::function<Property* ()>;

		static PropertyFactory& instance(void);
		static Property* createProperty(const ConstJsonObject& _jsonObject);
		static Property* createProperty(const std::string& _propertyType);

		static bool registerProperty(const std::string& _type, const PropertyConstructor& _constructor);

	private:
		PropertyFactory();
		~PropertyFactory();

		std::map<std::string, PropertyConstructor> m_constructors;
	};
}