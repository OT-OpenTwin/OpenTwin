// @otlicense
// File: EntityNameValidator.cpp
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
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Syntax/EntityNameValidator.h"

ot::EntityNameValidator::EntityNameValidator() 
	: m_charBlackList({ QChar('/') })
{
	
}

QValidator::State ot::EntityNameValidator::validateString(const QString& _str) const {
	for (const QChar& c : _str) {
		if (isCharInvalid(c)) {
			return QValidator::State::Invalid;
		}
	}

	return QValidator::State::Acceptable;
}

void ot::EntityNameValidator::fixup(QString& _input, int& _cursorPosition) const {
	int ix = 0;
	int len = _input.size();
	
	for (int i = 0; i < len; ) {
		if (isCharInvalid(_input.at(i))) {
			_input.removeAt(i);
			len--;

			if (i < _cursorPosition) {
				_cursorPosition--;
			}
		}
		else {
			i++;
		}
	}
}

inline bool ot::EntityNameValidator::isCharInvalid(QChar _c) const
{
	return std::find(m_charBlackList.begin(), m_charBlackList.end(), _c) != m_charBlackList.end();
}
