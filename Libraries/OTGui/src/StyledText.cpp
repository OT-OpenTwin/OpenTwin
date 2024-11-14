//! @file StyledText.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/StyledText.h"

std::string ot::StyledText::toString(ColorReference _colorReference) {
	switch (_colorReference) {
	case ColorReference::Default: return "Default";
	case ColorReference::Highlight: return "Highlight";
	case ColorReference::LightHighlight: return "LightHighlight";
	case ColorReference::Comment: return "Comment";
	case ColorReference::Warning: return "Warning";
	case ColorReference::Error: return "Error";
	default:
		OT_LOG_EAS("Unknown ColorReference (" + std::to_string((int)_colorReference) + ")");
		return "Default";
	}
}

ot::StyledText::ColorReference ot::StyledText::stringToColorReference(const std::string& _colorReference) {
	if (_colorReference == toString(ColorReference::Default)) return ColorReference::Default;
	else if (_colorReference == toString(ColorReference::Highlight)) return ColorReference::Highlight;
	else if (_colorReference == toString(ColorReference::LightHighlight)) return ColorReference::LightHighlight;
	else if (_colorReference == toString(ColorReference::Comment)) return ColorReference::Comment;
	else if (_colorReference == toString(ColorReference::Warning)) return ColorReference::Warning;
	else if (_colorReference == toString(ColorReference::Error)) return ColorReference::Error;
	else {
		OT_LOG_EAS("Unknown ColorReference \"" + _colorReference + "\"");
		return ColorReference::Default;
	}
}
