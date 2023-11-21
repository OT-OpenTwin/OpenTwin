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
		Preferred, //! The item size is the ideal size, the item wont shrink but may expand
		Dynamic //! The item size will be ignored and the requested size will be used instead (only use for nested items)
	};

	//! @brief Describes in which direction a connetion from an item may face
	enum ConnectionDirection {
		ConnectAny, //! A connection from this item may go in any direction
		ConnectLeft, //! Connection starts towards left
		ConnectUp, //! Connection starts upwards
		ConnectRight, //! Connection starts towards right
		ConnectDown //! Connection starts downwards
	};

	//! @brief Describes how the painter will behave when painting outsite the gradient area
	enum GradientSpread {
		PadSpread, //! Pad the outside area with the color at the closest stop point
		RepeatSpread, //! Repeat the gradient
		ReflectSpread //! Reflect the gradient
	};

	OT_GUI_API_EXPORT std::string toString(Alignment _alignment);
	OT_GUI_API_EXPORT Alignment stringToAlignment(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(Orientation _orientation);
	OT_GUI_API_EXPORT Orientation stringToOrientation(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(FontFamily _fontFamily);
	OT_GUI_API_EXPORT FontFamily stringToFontFamily(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(SizePolicy _policy);
	OT_GUI_API_EXPORT SizePolicy stringToSizePolicy(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(ConnectionDirection _direction);
	OT_GUI_API_EXPORT ConnectionDirection stringToConnectionDirection(const std::string& _direction);

	OT_GUI_API_EXPORT std::string toString(GradientSpread _spread);
	OT_GUI_API_EXPORT GradientSpread stringToGradientSpread(const std::string& _spread);
}