//! @file PropertyDialogCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyDialogCfg.h"

ot::PropertyDialogCfg::PropertyDialogCfg() {
	this->setMinSize(800, 600);
}

ot::PropertyDialogCfg::~PropertyDialogCfg() {
	
}

void ot::PropertyDialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	DialogCfg::addToJsonObject(_object, _allocator);

	JsonObject cfgObj;
	m_gridConfig.addToJsonObject(cfgObj, _allocator);
	_object.AddMember("Config", cfgObj, _allocator);
}

void ot::PropertyDialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	DialogCfg::setFromJsonObject(_object);

	ConstJsonObject cfgObj = json::getObject(_object, "Config");
	m_gridConfig.setFromJsonObject(cfgObj);
}
