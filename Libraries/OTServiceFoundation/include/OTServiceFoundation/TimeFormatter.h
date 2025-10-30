// @otlicense
// File: TimeFormatter.h
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
#include <chrono>
#include <string>

class __declspec(dllexport) TimeFormatter
{
public:
	static std::string formatDuration(const std::chrono::steady_clock::time_point _begin, const std::chrono::steady_clock::time_point& _end);
	static const std::string createCurrentDateTimeString();
};
