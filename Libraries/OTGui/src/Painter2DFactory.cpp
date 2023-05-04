#include "OTGui/Painter2DFactory.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/rJSONHelper.h"

ot::Painter2D* ot::Painter2DFactory::painter2DFromType(const std::string& _type) {
	if (_type == OT_LINEARGRADIENTPAINTER2D_TYPE) {
		return new LinearGradientPainter2D;
	}
	else if (_type == OT_FILLPAINTER2D_TYPE) {
		return new FillPainter2D;
	}
	else {
		otAssert(0, "Unknown block type");
		throw std::exception("Invalid type");
	}
}

ot::Painter2D* ot::Painter2DFactory::painter2DFromJson(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "Type", String);
	ot::Painter2D* newPainter = painter2DFromType(_object["Type"].GetString());
	try {
		newPainter->setFromJsonObject(_object);
		return newPainter;
	}
	catch (const std::exception& _e) {
		delete newPainter;
		throw _e;
	}
	catch (...) {
		delete newPainter;
		throw std::exception("Fatal: Unknown error");
	}
}