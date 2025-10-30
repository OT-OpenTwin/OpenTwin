// @otlicense
// File: LoadInformation.h
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

// OpenTwin header
#include "OTCore/JSON.h"

//! @brief The LoadInformation class provides information about the current system load.
class LoadInformation {
public:
	typedef unsigned long long memory_t;
	typedef double load_t;
	static load_t maxLoadValue(void) { return DBL_MAX; }

	LoadInformation();
	LoadInformation(const LoadInformation& _other);
	LoadInformation(LoadInformation&& _other) noexcept;
	virtual ~LoadInformation();

	LoadInformation& operator = (const LoadInformation& _other);
	LoadInformation& operator = (LoadInformation&& _other) noexcept;

	load_t load(void) const;
	bool updateSystemUsageValues(ot::JsonDocument& _jsonDocument);

private:
	load_t calculateLoad(memory_t _total, memory_t _avail);

	load_t   m_currentPhysicalMemoryLoad;
	load_t   m_currentVirtualMemoryLoad;

	memory_t m_totalPhysicalMemory;
	memory_t m_availablePhysicalMemory;
	memory_t m_totalVirtualMemory;
	memory_t m_availableVirtualMemory;
};