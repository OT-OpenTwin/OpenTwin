// @otlicense
// File: TestSolver3DEySourceTimeSteps2.cpp
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

TEST_P(FixtureSimulation3D, TestEySourceTimesteps2StimulationCurlEx)
{
	Alignment alignment = GetParam();
	CreateAirCubeEyCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	SimulateTimestep(1);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float exValue = *degreesOfFreedom->GetElectricVoltageXComponent(index);
				EXPECT_FLOAT_EQ(exValue, 0.f);
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestEySourceTimesteps2StimulationCurlEy)
{
	Alignment alignment = GetParam();
	CreateAirCubeEyCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	SimulateTimestep(1);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float eyValue = *degreesOfFreedom->GetElectricVoltageYComponent(index);
				if (index != GetSourceIndex())
				{
					EXPECT_FLOAT_EQ(eyValue, 0.f);
				}
				else
				{
					EXPECT_NE(eyValue, 0.f);
				}
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestEySourceTimesteps2StimulationCurlEz)
{
	Alignment alignment = GetParam();
	CreateAirCubeEyCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	SimulateTimestep(1);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float ezValue = *degreesOfFreedom->GetElectricVoltageZComponent(index);
				EXPECT_FLOAT_EQ(ezValue, 0.f);
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestEySourceTimesteps2StimulationCurlHx)
{
	Alignment alignment = GetParam();
	CreateAirCubeEyCentredHardwiredSolver(alignment);
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();

	SimulateTimestep(0);
	SimulateTimestep(1);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	index_t expectedIndex1 = GetSimulator()->GetIndexToCoordinate(x, y, z);
	index_t expectedIndex2 = GetSimulator()->GetIndexToCoordinate(x, y, z - 1);

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float hxValue = *degreesOfFreedom->GetMagneticVoltageXComponent(index);
				if (index != expectedIndex1 && index != expectedIndex2)
				{
					EXPECT_FLOAT_EQ(hxValue, 0.f);
				}
				else
				{
					EXPECT_NE(hxValue, 0.f);
				}
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestEySourceTimesteps2StimulationCurlHy)
{
	Alignment alignment = GetParam();
	CreateAirCubeEyCentredHardwiredSolver(alignment);
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();

	SimulateTimestep(0);
	SimulateTimestep(1);
	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();


	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float hyValue = *degreesOfFreedom->GetMagneticVoltageYComponent(index);
				EXPECT_FLOAT_EQ(hyValue, 0.f);
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestEySourceTimesteps2StimulationCurlHz)
{
	Alignment alignment = GetParam();
	CreateAirCubeEyCentredHardwiredSolver(alignment);
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();

	//expectedIndex1 -> (x , y, z), but because of the potential vectorlength != 1 it is solved like this:
	index_t expectedIndex1 = GetSimulator()->GetIndexToCoordinate(x- 1 , y, z) ;
	index_t expectedIndex2 = GetSimulator()->GetIndexToCoordinate(x, y, z);

	SimulateTimestep(0);
	SimulateTimestep(1);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float hzValue = *degreesOfFreedom->GetMagneticVoltageZComponent(index);
				if (index != expectedIndex1 && index != expectedIndex2)
				{
					EXPECT_FLOAT_EQ(hzValue, 0.f);
				}
				else
				{
					EXPECT_NE(hzValue, 0.f);
				}
				index++;
			}
		}
	}
}