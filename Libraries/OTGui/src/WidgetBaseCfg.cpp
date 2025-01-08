//! @file WidgetBaseCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/WidgetBaseCfg.h"

ot::WidgetBaseCfg::WidgetBaseCfg() {

}

ot::WidgetBaseCfg::WidgetBaseCfg(const std::string& _name) :
	m_name(_name)
{

}

ot::WidgetBaseCfg::~WidgetBaseCfg() {

}

void ot::WidgetBaseCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("ToolTip", JsonString(m_toolTip, _allocator), _allocator);
}

void ot::WidgetBaseCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_toolTip = json::getString(_object, "ToolTip");
}
