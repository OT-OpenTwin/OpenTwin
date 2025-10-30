// @otlicense
// File: UrlEncoding.cpp
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

#include "OTSystem/UrlEncoding.h"

#include <sstream>
#include <regex>
#include <iomanip>

std::string ot::url::urlEncode(std::string _str)
{
	std::ostringstream oss;
	std::regex r("[!'\\(\\)*-.0-9A-Za-z_~]");

	for (char& c : _str)
	{
		std::string s;
		s.push_back(c);

		if (std::regex_match(s, r))
		{
			oss << c;
		}
		else
		{
			oss << "%" << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (int)(c & 0xff);
		}
	}
	return oss.str();
}
