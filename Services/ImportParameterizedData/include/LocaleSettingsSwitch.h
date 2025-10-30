// @otlicense
// File: LocaleSettingsSwitch.h
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
#include <locale>
class LocaleSettingsSwitch
{
public:
	LocaleSettingsSwitch(const char intendedDecimalSeparator)
	{
		const char currentSeparator = *std::localeconv()->decimal_point;
		if (intendedDecimalSeparator != currentSeparator)
		{
			_reset = true;
			char* local = std::setlocale(LC_NUMERIC, nullptr);
			if (local == nullptr)
			{
				throw std::exception("Failed to determine active locale settings for decimal separator.");
			}
			else
			{
				_oldLocale = std::string(local);
			}
			if (intendedDecimalSeparator == '.')
			{
				std::setlocale(LC_NUMERIC, "en_US.UTF8");
			}
			else if(intendedDecimalSeparator == ',')
			{
				std::setlocale(LC_NUMERIC, "de_DE.UTF-8");
			}
			else
			{
				throw std::exception("Intended decimal separator is not supported.");
			}
		}
	}
	~LocaleSettingsSwitch()
	{
		if (_reset)
		{
			std::setlocale(LC_NUMERIC, _oldLocale.c_str());
		}
	}
private:
	std::string _oldLocale = "";
	bool _reset = false;
};
