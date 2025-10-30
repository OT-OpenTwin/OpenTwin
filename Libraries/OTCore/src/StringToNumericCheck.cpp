// @otlicense
// File: StringToNumericCheck.cpp
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

#include "OTCore/StringToNumericCheck.h"
#include <stdint.h>
#include <charconv>

bool ot::StringToNumericCheck::fitsInInt32(const std::string& str)
{
	int32_t value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	return ec == std::errc() && ptr == str.data() + str.size();
}

bool ot::StringToNumericCheck::fitsInInt64(const std::string& str)
{
	int64_t value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	return ec == std::errc() && ptr == str.data() + str.size();
}

bool ot::StringToNumericCheck::fitsInFloat(const std::string& str)
{
	float value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	bool fits = ec == std::errc() && ptr == str.data() + str.size();
	if (!fits)
	{
		auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value, std::chars_format::scientific);
		fits = ec == std::errc() && ptr == str.data() + str.size();
	}
	return fits;
}

bool ot::StringToNumericCheck::fitsInDouble(const std::string& str)
{
	double value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	bool fits = ec == std::errc() && ptr == str.data() + str.size();
	if (!fits)
	{
		auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value, std::chars_format::scientific);
		fits = ec == std::errc() && ptr == str.data() + str.size();
	}
	return fits;
}
