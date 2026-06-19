// @otlicense
// File: TestSolver3DValues.cpp
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
#include "MeasuredTargetValues.h"
#include <gtest/gtest.h>

TEST_P(FixtureSimulation3D, TestExSourceTimesteps1StimulationValues)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	SimulateTimestep(0);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();
	index_t index = GetSourceIndex();
	float exValue = *degreesOfFreedom->GetElectricVoltageXComponent(index);

	float expectedValue = _measuredTargetValuesSP.GetPortHardwiredGaussianT0();
	EXPECT_DOUBLE_EQ(expectedValue, exValue);
}

TEST_P(FixtureSimulation3D, TestExSourceTimesteps2StimulationValues)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	SimulateTimestep(1);
	
	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();

	index_t portIndex = GetSimulator()->GetIndexToCoordinate(x, y, z);

	float portValue = _measuredTargetValuesSP.GetPortHardwiredGaussianT1();
	float exValue = *degreesOfFreedom->GetElectricVoltageXComponent(portIndex);
	EXPECT_DOUBLE_EQ(portValue, exValue);

	float expectedHValues = _measuredTargetValuesSP.GetUniformGridCoefficientValue() * _measuredTargetValuesSP.GetPortHardwiredGaussianT0();
	float hyValue = *degreesOfFreedom->GetMagneticVoltageYComponent(portIndex);
	float hzValue = *degreesOfFreedom->GetMagneticVoltageZComponent(portIndex);
	EXPECT_DOUBLE_EQ(hyValue, expectedHValues);
	EXPECT_DOUBLE_EQ(hzValue, -expectedHValues);

	index_t hyIndex = GetSimulator()->GetIndexToCoordinate(x, y, z-1);
	index_t hzIndex = GetSimulator()->GetIndexToCoordinate(x, y -1, z);
	hyValue = *degreesOfFreedom->GetMagneticVoltageYComponent(hyIndex);
	hzValue = *degreesOfFreedom->GetMagneticVoltageZComponent(hzIndex);
	EXPECT_DOUBLE_EQ(hyValue, -expectedHValues);
	EXPECT_DOUBLE_EQ(hzValue, expectedHValues);
}

TEST_P(FixtureSimulation3D, TestEySourceTimesteps2StimulationValues)
{
	Alignment alignment = GetParam();
	CreateAirCubeEyCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	SimulateTimestep(1);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();

	index_t portIndex = GetSimulator()->GetIndexToCoordinate(x, y, z);

	float portValue = _measuredTargetValuesSP.GetPortHardwiredGaussianT1();
	float eyValue = *degreesOfFreedom->GetElectricVoltageYComponent(portIndex);
	EXPECT_DOUBLE_EQ(portValue, eyValue);

	float expectedHValues = _measuredTargetValuesSP.GetUniformGridCoefficientValue() * _measuredTargetValuesSP.GetPortHardwiredGaussianT0();
	float hxValue = *degreesOfFreedom->GetMagneticVoltageXComponent(portIndex);
	float hzValue = *degreesOfFreedom->GetMagneticVoltageZComponent(portIndex);
	EXPECT_DOUBLE_EQ(hxValue, -expectedHValues);
	EXPECT_DOUBLE_EQ(hzValue, expectedHValues);

	index_t hxIndex = GetSimulator()->GetIndexToCoordinate(x, y, z - 1);
	index_t hzIndex = GetSimulator()->GetIndexToCoordinate(x-1 , y, z) ;
	hxValue = *degreesOfFreedom->GetMagneticVoltageXComponent(hxIndex);
	hzValue = *degreesOfFreedom->GetMagneticVoltageZComponent(hzIndex);
	EXPECT_DOUBLE_EQ(hxValue, expectedHValues);
	EXPECT_DOUBLE_EQ(hzValue, -expectedHValues);
}

TEST_P(FixtureSimulation3D, TestHxSourceTimesteps2StimulationValues)
{
	Alignment alignment = GetParam();
	CreateAirCubeHxCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	SimulateTimestep(1);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();

	index_t portIndex = GetSimulator()->GetIndexToCoordinate(x, y, z);

	float portValue = _measuredTargetValuesSP.GetPortHardwiredGaussianT1();
	float hxValue = *degreesOfFreedom->GetMagneticVoltageXComponent(portIndex);
	EXPECT_DOUBLE_EQ(portValue, hxValue);

	float expectedEValues = _measuredTargetValuesSP.GetUniformGridDualCoefficientValue() * _measuredTargetValuesSP.GetPortHardwiredGaussianT0();
	float eyValue = *degreesOfFreedom->GetElectricVoltageYComponent(portIndex);
	float ezValue = *degreesOfFreedom->GetElectricVoltageZComponent(portIndex);
	EXPECT_DOUBLE_EQ(eyValue, expectedEValues);
	EXPECT_DOUBLE_EQ(ezValue, -expectedEValues);

	index_t eyIndex = GetSimulator()->GetIndexToCoordinate(x, y, z + 1);
	index_t ezIndex = GetSimulator()->GetIndexToCoordinate(x, y+1, z);
	eyValue = *degreesOfFreedom->GetElectricVoltageYComponent(eyIndex);
	ezValue = *degreesOfFreedom->GetElectricVoltageZComponent(ezIndex);
	EXPECT_DOUBLE_EQ(eyValue, -expectedEValues);
	EXPECT_DOUBLE_EQ(ezValue, expectedEValues);
}

TEST_P(FixtureSimulation3D, TestHySourceTimesteps2StimulationValues)
{
	Alignment alignment = GetParam();
	CreateAirCubeHyCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	SimulateTimestep(1);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();

	index_t portIndex = GetSimulator()->GetIndexToCoordinate(x, y, z);

	float portValue = _measuredTargetValuesSP.GetPortHardwiredGaussianT1();
	float hyValue = *degreesOfFreedom->GetMagneticVoltageYComponent(portIndex);
	EXPECT_DOUBLE_EQ(portValue, hyValue);

	float expectedEValues = _measuredTargetValuesSP.GetUniformGridDualCoefficientValue() * _measuredTargetValuesSP.GetPortHardwiredGaussianT0();
	float exValue = *degreesOfFreedom->GetElectricVoltageXComponent(portIndex);
	float ezValue = *degreesOfFreedom->GetElectricVoltageZComponent(portIndex);
	EXPECT_DOUBLE_EQ(exValue, -expectedEValues);
	EXPECT_DOUBLE_EQ(ezValue, expectedEValues);

	index_t exIndex = GetSimulator()->GetIndexToCoordinate(x, y, z + 1);
	index_t ezIndex = GetSimulator()->GetIndexToCoordinate(x +1, y, z) ;
	exValue = *degreesOfFreedom->GetElectricVoltageXComponent(exIndex);
	ezValue = *degreesOfFreedom->GetElectricVoltageZComponent(ezIndex);
	EXPECT_DOUBLE_EQ(exValue, expectedEValues);
	EXPECT_DOUBLE_EQ(ezValue, -expectedEValues);
}