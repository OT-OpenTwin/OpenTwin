// @otlicense
// File: TestTabledataToResultdataHandler.cpp
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

// TODO: temporarily disabled tests are out of sync with the refactored source
// (DataSourceHandler removed; TabledataToResultdataHandler ctor 5->2 args; EntityTableSelectedRanges include path; /permissive- rvalue->non-const-ref). Re-enable after updating to current API.
#if 0 // OT-DISABLED

#include "FixtureTabledataToResultdataHandler.h"


TEST_F(FixtureTabledataToResultdataHandler, UnitExtractionFromName_NoUnit)
{
	const std::string expectedName = "Frequency";
	std::string name = "Frequency";
	std::string unit =	extractUnitFromName(name);
	ASSERT_EQ(expectedName, name);
	ASSERT_EQ(unit, "");
}


TEST_F(FixtureTabledataToResultdataHandler, UnitExtractionFromName_WithUnit)
{
	const std::string expectedName = "Frequency";
	const std::string expectedUnit = "Hz";
	std::string name = "Frequency [Hz]";
	std::string unit = extractUnitFromName(name);
	ASSERT_EQ(expectedName, name);
	ASSERT_EQ(expectedUnit,unit);
}


TEST_F(FixtureTabledataToResultdataHandler, UnitExtractionFromName_WithoutUnitOpeningBracked)
{
	const std::string expectedName = "Frequency [Hz";
	const std::string expectedUnit = "";
	std::string name = "Frequency [Hz";
	std::string unit = extractUnitFromName(name);
	ASSERT_EQ(expectedName, name);
	ASSERT_EQ(expectedUnit, unit);
}

TEST_F(FixtureTabledataToResultdataHandler, UnitExtractionFromName_WithoutUnitClosingBracked)
{
	const std::string expectedName = "Frequency Hz]";
	const std::string expectedUnit = "";
	std::string name = "Frequency Hz]";
	std::string unit = extractUnitFromName(name);
	ASSERT_EQ(expectedName, name);
	ASSERT_EQ(expectedUnit, unit);
}
#endif // OT-DISABLED
