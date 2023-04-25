#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

namespace ot {

	enum Orientation {
		Center,
		Top,
		TopRight,
		Right,
		BottomRight,
		Bottom,
		BottomLeft,
		Left,
		TopLeft
	};

	OT_GUI_API_EXPORT std::string toString(Orientation _orientation);
	OT_GUI_API_EXPORT Orientation stringToOrientation(const std::string& _string);

}