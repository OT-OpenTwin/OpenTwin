//! @file WidgetViewCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/WidgetViewCfg.h"

// ###########################################################################################################################################################################################################################################################################################################################

ot::WidgetViewCfg::WidgetViewCfg() 
{

}

ot::WidgetViewCfg::~WidgetViewCfg() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::WidgetViewCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	WidgetViewBase::addToJsonObject(_object, _allocator);
	
	_object.AddMember(OT_JSON_MEMBER_WidgetViewCfgType, JsonString(this->getViewType(), _allocator), _allocator);
	_object.AddMember("Parent", JsonString(m_parentViewName, _allocator), _allocator);
}

void ot::WidgetViewCfg::setFromJsonObject(const ConstJsonObject& _object) {
	WidgetViewBase::setFromJsonObject(_object);

	m_parentViewName = json::getString(_object, "Parent");
}