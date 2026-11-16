// @otlicense
// File: StringToNumericCheck.h
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

#include "OTCore/CoreAPIExport.h"

#include <string>

namespace ot
{
	namespace StringToNumericCheck
	{
		bool OT_CORE_API_EXPORTONLY fitsInInt32(const std::string& str);
		bool OT_CORE_API_EXPORTONLY fitsInInt64(const std::string& str);
		bool OT_CORE_API_EXPORTONLY fitsInFloat(const std::string& str);
		bool OT_CORE_API_EXPORTONLY fitsInDouble(const std::string& str);
		bool OT_CORE_API_EXPORTONLY isNotANumber(const std::string& str);
	}
}
