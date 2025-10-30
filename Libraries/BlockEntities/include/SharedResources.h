// @otlicense
// File: SharedResources.h
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
#include <string>
namespace BlockEntities
{
	struct __declspec(dllexport) SharedResources
	{
		static std::string getCornerImagePath() { return "Images/"; }
		static std::string getCornerImageNameDB() {return "DataBase.svg";}
		static std::string getCornerImageNameVis() { return "Visualisation.svg"; }
		static std::string getCornerImageNamePython() { return "Python.svg"; }
	};
}
