//! @file PropertyInputFactory.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Property.h"
#include "OTWidgets/PropertyInput.h"

namespace ot {

	namespace PropertyInputFactory {
		
		PropertyInput* createInput(const Property* _config);

	}

}