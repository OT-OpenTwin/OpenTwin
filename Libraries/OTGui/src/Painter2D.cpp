//! @file Painter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Painter2D.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

void ot::Painter2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, OT_SimpleFactoryJsonKey, this->simpleFactoryObjectKey());
}

void ot::Painter2D::setFromJsonObject(OT_rJSON_val& _object) {

}