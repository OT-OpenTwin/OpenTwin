//! @file Painter2DFactory.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Painter2DFactory.h"

ot::Painter2DFactory& ot::Painter2DFactory::instance(void) {
	static Painter2DFactory g_instance;
	return g_instance;
}

ot::Painter2D* ot::Painter2DFactory::create(const ConstJsonObject& _jsonObject) {
	return this->createFromJSON(_jsonObject, OT_JSON_MEMBER_Painter2DType);
}