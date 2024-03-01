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
#include "OTGui/PropertyDirectory.h"
#include "OTGui/PropertyFilePath.h"

#include "OTWidgets/PropertyInputFactory.h"
#include "OTWidgets/PropertyInputBool.h"
#include "OTWidgets/PropertyInputColor.h"
#include "OTWidgets/PropertyInputDouble.h"
#include "OTWidgets/PropertyInputDirectory.h"
#include "OTWidgets/PropertyInputFilePath.h"
#include "OTWidgets/PropertyInputInt.h"
#include "OTWidgets/PropertyInputString.h"
#include "OTWidgets/PropertyInputStringList.h"

ot::PropertyInput* ot::PropertyInputFactory::createInput(const Property* _config) {
	switch (_config->getPropertyType())
	{
	case ot::Property::BoolType:
	{
		const ot::PropertyBool* prop = dynamic_cast<const ot::PropertyBool*>(_config);
		if (!prop) {
			OT_LOG_EA("Property cast failed");
			return nullptr;
		}
		return new PropertyInputBool(prop);
	}
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
	{
		const ot::PropertyDouble* prop = dynamic_cast<const ot::PropertyDouble*>(_config);
		if (!prop) {
			OT_LOG_EA("Property cast failed");
			return nullptr;
		}
		return new PropertyInputDouble(prop);
	}
	case ot::Property::StringType:
	{
		const ot::PropertyString* prop = dynamic_cast<const ot::PropertyString*>(_config);
		if (!prop) {
			OT_LOG_EA("Property cast failed");
			return nullptr;
		}
		return new PropertyInputString(prop);
	}
	case ot::Property::StringListType:
	{
		const ot::PropertyStringList* prop = dynamic_cast<const ot::PropertyStringList*>(_config);
		if (!prop) {
			OT_LOG_EA("Property cast failed");
			return nullptr;
		}
		return new PropertyInputStringList(prop);
	}
	case ot::Property::ColorType:
	{
		const ot::PropertyColor* prop = dynamic_cast<const ot::PropertyColor*>(_config);
		if (!prop) {
			OT_LOG_EA("Property cast failed");
			return nullptr;
		}
		return new PropertyInputColor(prop);
	}
	case ot::Property::FilePathType:
	{
		const ot::PropertyFilePath* prop = dynamic_cast<const ot::PropertyFilePath*>(_config);
		if (!prop) {
			OT_LOG_EA("Property cast failed");
			return nullptr;
		}
		return new PropertyInputFilePath(prop);
	}
	case ot::Property::DirectoryType:
	{
		const ot::PropertyDirectory* prop = dynamic_cast<const ot::PropertyDirectory*>(_config);
		if (!prop) {
			OT_LOG_EA("Property cast failed");
			return nullptr;
		}
		return new PropertyInputDirectory(prop);
	}
	default:
		OT_LOG_EA("Unknown property type");
		break;
	}
	return nullptr;
}
