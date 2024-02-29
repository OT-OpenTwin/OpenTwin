//! @file PropertyGridItemFactory.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"


namespace ot {

	class Property;
	class PropertyGridItem;

	namespace PropertyGridItemFactory {

		OT_WIDGETS_API_EXPORT PropertyGridItem* createPropertyGridItemFromConfig(Property* _property);

	}
}