//! @file PropertyFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyFactory.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/PropertyGroup.h"

ot::Property* ot::PropertyFactory::createProperty(const ConstJsonObject& _jsonObject) {
	std::string type = json::getString(_jsonObject, OT_JSON_MEMBER_Property_Type);
	Property* prop = PropertyFactory::createProperty(Property::stringToPropertyType(type));
	prop->setFromJsonObject(_jsonObject);
	return prop;
}

ot::Property* ot::PropertyFactory::createProperty(Property::PropertyType _propertyType) {
	switch (_propertyType)
	{
	case ot::Property::BoolType: return new PropertyBool();
	case ot::Property::IntType: return new PropertyInt();
	case ot::Property::DoubleType: return new PropertyDouble();
	case ot::Property::StringType: return new PropertyString();
	case ot::Property::StringListType: return new PropertyStringList();
	case ot::Property::ColorType: return new PropertyColor();
	default:
		OT_LOG_EAS("Unknown property type (" + std::to_string(_propertyType) + ")");
		return nullptr;
	}
}