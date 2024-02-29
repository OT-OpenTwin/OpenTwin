//! @file PropertyInputFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"

#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyStringList.h"

#include "OTWidgets/PropertyInputFactory.h"
#include "OTWidgets/PropertyInputInt.h"

ot::PropertyInput* ot::PropertyInputFactory::createInput(const Property* _config) {
	switch (_config->getPropertyType())
	{
	case ot::Property::BoolType:
		break;
	case ot::Property::IntType:
	{
		const ot::PropertyInt* prop = dynamic_cast<const ot::PropertyInt*>(_config);
		if (!prop) {
			OT_LOG_EA("Property cast failed");
			return nullptr;
		}
		return new PropertyInputInt(prop);
	}
	case ot::Property::DoubleType:
		break;
	case ot::Property::StringType:
		break;
	case ot::Property::StringListType:
		break;
	case ot::Property::ColorType:
		break;
	default:
		OT_LOG_EA("Unknown property type");
		break;
	}
	return nullptr;
}
