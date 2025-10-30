// @otlicense
// File: SystemDataAccess.h
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

#include "../Response/DataStorageResponse.h"

using string = std::string;

namespace DataStorageAPI
{
	class SystemDataAccess
	{
	public:
		__declspec(dllexport) SystemDataAccess(string databaseName, string collectionName);
		__declspec(dllexport) DataStorageResponse GetSystemDetails(int siteId);

	private:
		std::string _mongoDbName;
		std::string _mongoCollectionName;
	};
}

