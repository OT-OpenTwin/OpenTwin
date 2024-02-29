//! @file PropertyFactory.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/Property.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

namespace ot {
	namespace PropertyFactory {
		OT_GUI_API_EXPORT Property* createProperty(const ConstJsonObject& _jsonObject);
		OT_GUI_API_EXPORT Property* createProperty(Property::PropertyType _propertyType);
	}
}