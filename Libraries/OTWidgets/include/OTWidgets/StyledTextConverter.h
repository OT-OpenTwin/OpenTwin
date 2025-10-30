// @otlicense
// File: StyledTextConverter.h
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
#include "OTGui/StyledTextBuilder.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT StyledTextConverter {
	public:
		//! @brief Convert the provided StyledTextBuilder to HTML.
		//! @param _builder Builder to convert.
		//! @param _silent If true no error messages are printed if the conversion fails.
		static QString toHtml(const StyledTextBuilder& _builder, bool _silent = false);

	private:
		static void addHtmlSpan(const StyledTextStyle& _style, QString& _destination, const QString& _text, bool _silent);
		static QString getColorFromReference(StyledText::ColorReference _colorReference, bool _silent);
		static QString evaluateSubstitutionTokens(const std::string& _text, bool _silent);

		StyledTextConverter() = delete;
	};

}