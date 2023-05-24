#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

namespace ot {

	enum Orientation {
		OrientCenter,
		OrientTop,
		OrientTopRight,
		OrientRight,
		OrientBottomRight,
		OrientBottom,
		OrientBottomLeft,
		OrientLeft,
		OrientTopLeft
	};

	//! @brief Generally available font families
	enum FontFamily {
		Arial,
		ComicSansMS,
		Consolas,
		CourierNew,
		Georgia,
		Helvetica,
		Impact,
		Palatino,
		TimesNewRoman,
		TrebuchetMS,
		Verdana
	};

	OT_GUI_API_EXPORT std::string toString(Orientation _orientation);
	OT_GUI_API_EXPORT Orientation stringToOrientation(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(FontFamily _fontFamily);
	OT_GUI_API_EXPORT FontFamily stringToFontFamily(const std::string& _string);

}