// @otlicense
// File: UrlEncoding.h
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

#include "OTSystem/SystemAPIExport.h"

#include <string>

namespace ot {

	//! @namespace url
	namespace url {

		//! @brief Will encode the string given as argument such that it can be used in URLs.
		//! @param _str The string to be encoded
		//! @return The URL safe encoded string
		OT_SYS_API_EXPORT std::string urlEncode(std::string _str);
	}
}
