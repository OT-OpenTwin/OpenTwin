// @otlicense
// File: TypeNames.h
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

// std header
#include <string>
#include <stdint.h>

namespace ot
{
	class TypeNames
	{
	public:
		static std::string getInt32TypeName() { return "int32"; };
		static std::string getInt64TypeName() { return "int64"; };
		static std::string getStringTypeName() { return "string"; };
		static std::string getDoubleTypeName() { return "double"; };
		static std::string getFloatTypeName() { return "float"; };
		static std::string getCharTypeName() { return "char"; };
		static std::string getBoolTypeName() { return "bool"; };
	};
}