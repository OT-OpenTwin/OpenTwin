//! @file PropertyGridItemInt.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyInt.h"
#include "OTWidgets/PropertyGridItemInt.h"

ot::PropertyGridItemInt::PropertyGridItemInt() {

}

ot::PropertyGridItemInt::~PropertyGridItemInt() {

}

bool ot::PropertyGridItemInt::setupFromConfig(Property* _config) {
	PropertyInt* config = dynamic_cast<PropertyInt*>(_config);
	if (!config) {
		OT_LOG_EA("Property cast failed");
		return false;
	}



	return PropertyGridItem::setupFromConfig(_config);
}
