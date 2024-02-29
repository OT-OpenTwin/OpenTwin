//! @file PropertyGridItemStringList.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyStringList.h"
#include "OTWidgets/PropertyGridItemStringList.h"

ot::PropertyGridItemStringList::PropertyGridItemStringList() {

}

ot::PropertyGridItemStringList::~PropertyGridItemStringList() {

}

bool ot::PropertyGridItemStringList::setupFromConfig(Property* _config) {
	PropertyStringList* config = dynamic_cast<PropertyStringList*>(_config);
	if (!config) {
		OT_LOG_EA("Property cast failed");
		return false;
	}



	return PropertyGridItem::setupFromConfig(_config);
}
