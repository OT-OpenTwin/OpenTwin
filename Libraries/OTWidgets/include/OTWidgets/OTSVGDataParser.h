// @otlicense
// File: OTSVGDataParser.h
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

#pragma once

// OpenTwin header
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qbytearray.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT OTSVGDataParser {
		OT_DECL_STATICONLY(OTSVGDataParser)
	public:
		//! @brief Replaces OpenTwin style placeholders in the raw SVG data with the values from the current global color style.
		//! @param _rawData The raw SVG data containing style placeholders.
		//! @return The processed SVG data with the style placeholders replaced by actual color values.
		static QByteArray parse(const QByteArray& _rawData);

		//! @brief Replaces OpenTwin style placeholders in the raw SVG data with the values from the provided color style.
		//! @param _rawData The raw SVG data containing style placeholders.
		//! @param _style The ColorStyle object containing the actual color values to replace the placeholders.
		//! @return The processed SVG data with the style placeholders replaced by actual color values.
		static QByteArray parse(const QByteArray& _rawData, const ColorStyle& _style);

	private:
		enum PlaceholderType {
			PlaceholderType_Int,
			PlaceholderType_Color,
			PlaceholderType_Double
		};
		static void processPlaceholders(const QByteArray& _data, const ColorStyle& _style, PlaceholderType _type, std::map<QByteArray, QByteArray>& _replacements, int& _defIx, QByteArray& _gradientsData);
	};

}