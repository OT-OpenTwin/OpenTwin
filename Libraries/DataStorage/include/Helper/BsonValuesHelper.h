// @otlicense
// File: BsonValuesHelper.h
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
#include <iostream>
#include <string>
#include <bsoncxx/types/value.hpp>

using value = bsoncxx::types::value;

namespace DataStorageAPI
{
	class __declspec(dllexport) BsonValuesHelper
	{
	public:
		static value getInt32BsonValue(int32_t value);
		static int32_t getInt32FromBsonValue(value value);

		static value getInt64BsonValue(int64_t value);
		static int64_t getInt64FromBsonValue(value value);

		static std::string getStringFromBsonValue(value value);

		static value getBoolValue(bool value);
		static bool getBoolFromBsonValue(value value);

		static value getDoubleValue(double value);
		static double getDoubleFromBsonValue(value value);

		static value getOIdValue(std::string value);
		static std::string getOIdFromBsonValue(value value);

	private:
		BsonValuesHelper() = default;
	};
}

