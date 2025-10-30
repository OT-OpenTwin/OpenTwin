// @otlicense
// File: TextEncoding.cpp
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

#include "OTCore/TextEncoding.h"
#include <codecvt>


std::string ot::TextEncoding::getString(ot::TextEncoding::EncodingStandard type) const
{
	if (_encodingStandardToString.find(type) == _encodingStandardToString.end())
	{
		throw std::exception("Encoding standard not found");
	}
	else
	{
		return (*_encodingStandardToString.find(type)).second;
	}
}

ot::TextEncoding::EncodingStandard ot::TextEncoding::getType(const std::string& type) const
{
	if (_stringToEncodingStandard.find(type) == _stringToEncodingStandard.end())
	{
		throw std::exception("Encoding standard not found");
	}
	else
	{
		return (*_stringToEncodingStandard.find(type)).second;
	}
}
