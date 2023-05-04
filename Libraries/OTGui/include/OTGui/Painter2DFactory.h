//! @file Painter2DFactory.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/rJSON.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class Painter2D;

	namespace Painter2DFactory {

		//! @brief Creates a 2D painter for the given type
		//! @param _type A valid painter configuration type
		OT_GUI_API_EXPORT Painter2D* painter2DFromType(const std::string& _type);

		//! @brief Creates a 2D painter from the given JSON object
		//! @param _object The JSON object containing the painter configuration
		OT_GUI_API_EXPORT Painter2D* painter2DFromJson(OT_rJSON_val& _object);
	}
}