//! @file PropertyGridItemFactory.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"

#include "OTGui/Property.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyStringList.h"

#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridItemInt.h"
#include "OTWidgets/PropertyGridItemBool.h"
#include "OTWidgets/PropertyGridItemDouble.h"
#include "OTWidgets/PropertyGridItemString.h"
#include "OTWidgets/PropertyGridItemStringList.h"
#include "OTWidgets/PropertyGridItemFactory.h"

ot::PropertyGridItem* ot::PropertyGridItemFactory::createPropertyGridItemFromConfig(Property* _property) {
	ot::PropertyGridItem* ret = nullptr;
	switch (_property->getPropertyType())
	{
	case ot::Property::BoolType: ret = new PropertyGridItemBool; break;
	case ot::Property::IntType: ret = new PropertyGridItemInt; break;
	case ot::Property::DoubleType: ret = new PropertyGridItemDouble; break;
	case ot::Property::StringType: ret = new PropertyGridItemString; break;
	case ot::Property::StringListType: ret = new PropertyGridItemStringList; break;
	case ot::Property::ColorType: break;
	default:
		OT_LOG_EAS("Unknown property type (" + std::to_string((int)_property->getPropertyType()) + ")");
	}

	if (ret) {
		ret->setupFromConfig(_property);
	}
	return ret;
}