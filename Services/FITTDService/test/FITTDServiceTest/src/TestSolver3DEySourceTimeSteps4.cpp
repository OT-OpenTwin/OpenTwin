// @otlicense
// File: TestSolver3DEySourceTimeSteps4.cpp
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

#include "FixtureSimulation3D.h"
#include <gtest/gtest.h>

TEST_P(FixtureSimulation3D, TestEySourceTimesteps4StimulationCurlEx)
{
	Alignment alignment = GetParam();
	CreateAirCubeEyCentredHardwiredSolver(alignment);
	uint64_t sourceX = GetSourceCoordinateX();
	uint64_t sourceY = GetSourceCoordinateY();
	uint64_t sourceZ = GetSourceCoordinateZ();

	uint64_t expectedIndex1 = GetSimulator()->GetIndexToCoordinate(sourceX, sourceY, sourceZ);
	uint64_t expectedIndex2 = GetSimulator()->GetIndexToCoordinate(sourceX+1, sourceY, sourceZ);
	uint64_t expectedIndex3 = GetSimulator()->GetIndexToCoordinate(sourceX + 1, sourceY, sourceZ-1);
	uint64_t expectedIndex4 = GetSimulator()->GetIndexToCoordinate(sourceX , sourceY, sourceZ -1);
	uint64_t expectedIndex5 = GetSimulator()->GetIndexToCoordinate(sourceX + 1, sourceY -1, sourceZ);
	uint64_t expectedIndex6 = GetSimulator()->GetIndexToCoordinate(sourceX + 1, sourceY-1, sourceZ-1);
	uint64_t expectedIndex7 = GetSimulator()->GetIndexToCoordinate(sourceX , sourceY-1, sourceZ);
	uint64_t expectedIndex8 = GetSimulator()->GetIndexToCoordinate(sourceX, sourceY-1, sourceZ-1);

	SimulateTimestep(0);
	SimulateTimestep(1);
	SimulateTimestep(2);
	SimulateTimestep(3);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (uint64_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (uint64_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			uint64_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (uint64_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float hxValue = *degreesOfFreedom->GetMagneticVoltageXComponent(index);
				if (index != expectedIndex1 && index != expectedIndex2 && index != expectedIndex3 && index != expectedIndex4 
					&& index != expectedIndex5 && index != expectedIndex6 && index != expectedIndex7 && index != expectedIndex8)
				{
					EXPECT_LE(abs(hxValue), FLT_EPSILON) << "Index: " << index;
				}
				else
				{
					EXPECT_NE(hxValue, 0.f) << "Index: " << index;
				}
				index++;
			}
		}
	}

}