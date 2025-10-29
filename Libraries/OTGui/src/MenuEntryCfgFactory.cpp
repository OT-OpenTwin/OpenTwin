// @otlicense

//! @file MenuEntryCfgFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/MenuEntryCfgFactory.h"

ot::MenuEntryCfgFactory& ot::MenuEntryCfgFactory::instance(void) {
	static MenuEntryCfgFactory g_instance;
	return g_instance;
}

ot::MenuEntryCfg* ot::MenuEntryCfgFactory::create(const ConstJsonObject& _jsonObject) {
	return MenuEntryCfgFactory::instance().createFromJSON(_jsonObject, MenuEntryCfg::entryTypeJsonKey());
}

ot::MenuEntryCfgFactory::MenuEntryCfgFactory() {

}

ot::MenuEntryCfgFactory::~MenuEntryCfgFactory() {

}
