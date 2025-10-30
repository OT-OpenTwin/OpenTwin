// @otlicense

// OpenTwin header
#include "OTGui/Painter2DFactory.h"

ot::Painter2DFactory& ot::Painter2DFactory::instance(void) {
	static Painter2DFactory g_instance;
	return g_instance;
}

ot::Painter2D* ot::Painter2DFactory::create(const ConstJsonObject& _jsonObject) {
	return Painter2DFactory::instance().createFromJSON(_jsonObject, OT_JSON_MEMBER_Painter2DType);
}