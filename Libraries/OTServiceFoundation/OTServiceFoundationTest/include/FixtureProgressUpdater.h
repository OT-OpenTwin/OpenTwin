// @otlicense
// File: FixtureProgressUpdater.h
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
#include <gtest/gtest.h>
#include "OTServiceFoundation/ProgressUpdater.h"

class FixtureProgressUpdater : public testing::Test
{
public:
	FixtureProgressUpdater();
	uint64_t getTriggerFrequency() const;
	ProgressUpdater& getUpdater() { return m_updater; };

private:
	ProgressUpdater m_updater;
};
