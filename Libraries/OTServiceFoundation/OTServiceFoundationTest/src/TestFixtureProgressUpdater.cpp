// @otlicense
// File: TestFixtureProgressUpdater.cpp
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

#include <gtest/gtest.h>
#include "FixtureProgressUpdater.h"


TEST_F(FixtureProgressUpdater, TriggerFrequency_NbUpdatesGTNbSteps)
{
	ProgressUpdater& updater = getUpdater();
	updater.setTotalNumberOfUpdates(5, 3);
	EXPECT_EQ(getTriggerFrequency(), 1);
}

TEST_F(FixtureProgressUpdater, TriggerFrequency_NbUpdatesLTNbSteps)
{
	ProgressUpdater& updater = getUpdater();
	updater.setTotalNumberOfUpdates(2, 5);
	EXPECT_EQ(getTriggerFrequency(), 2);
}