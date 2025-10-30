// @otlicense
// File: TimeFormatter.cpp
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

#include "OTServiceFoundation/TimeFormatter.h"

std::string TimeFormatter::formatDuration(const std::chrono::steady_clock::time_point _begin, const std::chrono::steady_clock::time_point& _end)
{
	const std::chrono::duration<double, std::milli> duration_ms = (_end - _begin);
	if(duration_ms.count() < 1000.0)
	{
		return std::to_string(static_cast<uint64_t>(duration_ms.count())) + " ms";
	}
	else if(duration_ms.count() < 60000.0)
	{
		const double duration_s = duration_ms.count() / 1000.0;
		return std::to_string(static_cast<uint64_t>(duration_s)) + " s";
	}
	else if(duration_ms.count() < 3600000.0)
	{
		const double duration_min = duration_ms.count() / 60000.0;
		return std::to_string(static_cast<uint64_t>(duration_min)) + " min";
	}
	else
	{
		const double duration_h = duration_ms.count() / 3600000.0;
		return std::to_string(static_cast<uint64_t>(duration_h)) + " h";
	}
	
}

const std::string TimeFormatter::createCurrentDateTimeString()
{
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	const std::string timeAndDate = std::ctime(&now_time);
	return timeAndDate;
}
