// @otlicense
// File: EntityNameValidator.h
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
#include "OTWidgets/Syntax/CustomValidator.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT EntityNameValidator : public CustomValidator {
		OT_DECL_DEFCOPY(EntityNameValidator)
		OT_DECL_DEFMOVE(EntityNameValidator)
	public:
		EntityNameValidator();
		virtual ~EntityNameValidator() = default;

		virtual QValidator::State validate(QString& _input, int& _position) const override { return this->validateString(_input); };

		//! @brief Returns the validation state of the input string.
		//! @param _str String to check.
		QValidator::State validateString(const QString& _str) const;

		//! @brief Returns true if the input string is valid.
		bool isStringValid(const QString& _string) const { return this->validateString(_string) == QValidator::Acceptable; };

		virtual void fixup(QString& _input, int& _cursorPosition) const override;
	private:
		inline bool isCharInvalid(QChar _c) const;

		std::vector<QChar> m_charBlackList;
	};

}