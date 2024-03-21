//! @file PlainTextEditViewCfg.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PlainTextEditViewCfg.h"
#include "OTGui/WidgetViewCfgRegistrar.h"

static ot::WidgetViewCfgRegistrar<ot::PlainTextEditViewCfg> PlainTextEditViewCfgRegistrar(OT_WIDGETTYPE_PlainTextEditView);

ot::PlainTextEditViewCfg::PlainTextEditViewCfg() {

}

ot::PlainTextEditViewCfg::~PlainTextEditViewCfg() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::PlainTextEditViewCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	WidgetViewCfg::addToJsonObject(_object, _allocator);

}

void ot::PlainTextEditViewCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	WidgetViewCfg::setFromJsonObject(_object);

}