// @otlicense
// File: SourceTarget.h
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
#include <map>
#include <string>
enum sourceType { e, h };
enum axis { x_axis, y_axis, z_axis };

namespace port
{
	static std::map<axis, std::string> axisName = { {x_axis, "X-Axis"}, {y_axis, "Y-Axis"}, {z_axis, "Z-Axis"} };
	static std::map<axis, std::string> axisAbbreviation = { {x_axis, "x"}, {y_axis, "y"}, {z_axis, "z"} };
}

