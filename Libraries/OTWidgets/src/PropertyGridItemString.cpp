//! @file PropertyGridItemString.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyString.h"
#include "OTWidgets/PropertyGridItemString.h"

ot::PropertyGridItemString::PropertyGridItemString() {

}

ot::PropertyGridItemString::~PropertyGridItemString() {

}

bool ot::PropertyGridItemString::setupFromConfig(Property* _config) {
	PropertyString* config = dynamic_cast<PropertyString*>(_config);
	if (!config) {
		OT_LOG_EA("Property cast failed");
		return false;
	}



	return PropertyGridItem::setupFromConfig(_config);
}
