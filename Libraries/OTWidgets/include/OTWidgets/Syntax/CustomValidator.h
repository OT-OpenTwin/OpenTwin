// @otlicense
// File: CustomValidator.h
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
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtGui/qvalidator.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CustomValidator : public QValidator {
		OT_DECL_NOCOPY(CustomValidator)
		OT_DECL_NOMOVE(CustomValidator)
	public:
		CustomValidator() = default;
		virtual ~CustomValidator() = default;

		//! @brief Fixes the given string.
		//! Will call the fixup function with cursor position ignored.
		//! @ref fixup(QString&, int&)
		//! @param _input Input string to fix.
		virtual void fixup(QString& _input) const override;

		//! @brief Fixes the given string.
		//! @param _input Input string to fix.
		//! @param _cursorPosition Current cursor position in the string. This position should be updated if characters are added/removed before this position.
		virtual void fixup(QString& _input, int& _cursorPosition) const = 0;
	};

}