//! @file PropertyGridItemBool.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyBool.h"
#include "OTWidgets/PropertyGridItemBool.h"

ot::PropertyGridItemBool::PropertyGridItemBool() {

}

ot::PropertyGridItemBool::~PropertyGridItemBool() {

}

bool ot::PropertyGridItemBool::setupFromConfig(Property* _config) {
	PropertyBool* config = dynamic_cast<PropertyBool*>(_config);
	if (!config) {
		OT_LOG_EA("Property cast failed");
		return false;
	}



	return PropertyGridItem::setupFromConfig(_config);
}
