// @otlicense
// File: StyledText.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/StyledText.h"

std::string ot::StyledText::toString(SubstitutionToken _substitutionToken) {
	switch (_substitutionToken) {
	case ot::StyledText::EmptyToken: return "";
	case ot::StyledText::DateYYYYMMDD: return "DATE_YYYYMMDD";
	case ot::StyledText::TimeHHMMSS: return "TIME_HHMMSS";
	case ot::StyledText::TimeHHMMSSZZZZ: return "TIME_HHMMSSZZZZ";
	default:
		OT_LOG_EAS("Unknown substitution token (" + std::to_string(static_cast<int>(_substitutionToken)) + ")");
		return "";
	}
}

ot::StyledText::SubstitutionToken ot::StyledText::stringToSubstitutionToken(const std::string& _substitutionToken) {
	if (_substitutionToken == toString(SubstitutionToken::EmptyToken)) return SubstitutionToken::EmptyToken;
	else if (_substitutionToken == toString(SubstitutionToken::DateYYYYMMDD)) return SubstitutionToken::DateYYYYMMDD;
	else if (_substitutionToken == toString(SubstitutionToken::TimeHHMMSS)) return SubstitutionToken::TimeHHMMSS;
	else if (_substitutionToken == toString(SubstitutionToken::TimeHHMMSSZZZZ)) return SubstitutionToken::TimeHHMMSSZZZZ;
	else {
		OT_LOG_EAS("Unknown substitution token \"" + _substitutionToken + "\"");
		return SubstitutionToken::EmptyToken;
	}
}

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
