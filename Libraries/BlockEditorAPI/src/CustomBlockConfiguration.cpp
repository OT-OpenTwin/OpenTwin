// OpenTwin header
#include "OTBlockEditorAPI/CustomBlockConfiguration.h"

ot::CustomBlockConfiguration::CustomBlockConfiguration() {

}

ot::CustomBlockConfiguration::~CustomBlockConfiguration() {

}

void ot::CustomBlockConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockConfiguration::addToJsonObject(_document, _object);

}

void ot::CustomBlockConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockConfiguration::setFromJsonObject(_object);

}
