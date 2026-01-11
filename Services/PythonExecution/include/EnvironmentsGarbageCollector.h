// @otlicense
// File: EnvironmentsGarbageCollector.h
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

#include "WorkerWaiterState.h"
#include "InterpreterPathSettings.h"

class EnvironmentsGarbageCollector
{
public:
	EnvironmentsGarbageCollector() = default;

	void setInterpreterPathSettings(const InterpreterPathSettings& _settings) { m_interpreterPathSettings = _settings; }
	void run();
private:
	InterpreterPathSettings m_interpreterPathSettings;
	const int32_t m_daysUntilRemoval = 3;
	const int32_t m_numberOfEnvironmentsLimit = 5;
	const std::string m_removalMarker = "_remove";
	// All custom environments beyond the limit are marked for cleanup if they have not been accessed for m_daysUntilRemoval days
	void markForCleanup();
	void removeMarkedEnvironments();
};
