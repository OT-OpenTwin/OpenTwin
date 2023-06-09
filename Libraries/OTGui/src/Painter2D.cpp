#include "OTGui/Painter2D.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

void ot::Painter2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, "Type", painterType());
}

void ot::Painter2D::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "Type", String);
	std::string type = _object["Type"].GetString();
	if (type != painterType()) {
		otAssert(0, "Invalid Painter2D Type for import");
		OT_LOG_E("Invalid type \"" + type + "\", expected: \"" + painterType() + "\"");
		throw std::exception("Invalid Painter2D type");
	}
}