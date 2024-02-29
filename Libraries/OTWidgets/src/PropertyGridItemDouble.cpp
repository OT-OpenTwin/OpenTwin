//! @file PropertyGridItemDouble.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyDouble.h"
#include "OTWidgets/PropertyGridItemDouble.h"

ot::PropertyGridItemDouble::PropertyGridItemDouble() {
	
}

ot::PropertyGridItemDouble::~PropertyGridItemDouble() {

}

bool ot::PropertyGridItemDouble::setupFromConfig(Property* _config) {
	PropertyDouble* config = dynamic_cast<PropertyDouble*>(_config);
	if (!config) {
		OT_LOG_EA("Property cast failed");
		return false;
	}

	

	return PropertyGridItem::setupFromConfig(_config);
}
