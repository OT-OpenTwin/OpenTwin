#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

namespace ot {

	//! @brief Alignment
	enum Alignment {
		AlignCenter,
		AlignTop,
		AlignTopRight,
		AlignRight,
		AlignBottomRight,
		AlignBottom,
		AlignBottomLeft,
		AlignLeft,
		AlignTopLeft
	};

	//! @brief Orientation
	enum Orientation {
		Horizontal,
		Vertical
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

	//! @brief Size policy for UI elements
	enum SizePolicy {
		Fixed,	//! The size of the item will remain and wont be adjusted
		Shrink, //! The item size is the maximum size but the item may shrink
		Expand, //! The item size is the minimum size but the item may expand
		Dynamic //! The item size is the ideal size but the item may shrink and expand
	};

	OT_GUI_API_EXPORT std::string toString(Alignment _alignment);
	OT_GUI_API_EXPORT Alignment stringToAlignment(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(Orientation _orientation);
	OT_GUI_API_EXPORT Orientation stringToOrientation(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(FontFamily _fontFamily);
	OT_GUI_API_EXPORT FontFamily stringToFontFamily(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(SizePolicy _policy);
	OT_GUI_API_EXPORT SizePolicy stringToSizePolicy(const std::string& _string);

}