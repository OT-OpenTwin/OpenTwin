#include "FixtureSimulation3D.h"
#include <gtest/gtest.h>

TEST_P(FixtureSimulation3D, TestExSourceTimesteps1StimulationCurlEx)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float exValue = *degreesOfFreedom->GetElectricVoltageXComponent(index);
				if (index != GetSourceIndex())
				{
					EXPECT_FLOAT_EQ(exValue, 0.f);
				}
				else
				{
					EXPECT_NE(exValue, 0.f);
				}
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestExSourceTimesteps1StimulationCurlEy)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float eyValue = *degreesOfFreedom->GetElectricVoltageYComponent(index);
				EXPECT_FLOAT_EQ(eyValue, 0.f);
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestExSourceTimesteps1StimulationCurlEz)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
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

TEST_P(FixtureSimulation3D, TestExSourceTimesteps1StimulationCurlHx)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float hxValue = *degreesOfFreedom->GetMagneticVoltageXComponent(index);
				EXPECT_FLOAT_EQ(hxValue, 0.f);
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestExSourceTimesteps1StimulationCurlHy)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
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

TEST_P(FixtureSimulation3D, TestExSourceTimesteps1StimulationCurlHz)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float hzValue = *degreesOfFreedom->GetMagneticVoltageZComponent(index);
				EXPECT_FLOAT_EQ(hzValue, 0.f);
				index++;
			}
		}
	}
}
