#include "FixtureSimulation3D.h"
#include <gtest/gtest.h>
#include <list>

TEST_P(FixtureSimulation3D, TestExSourceTimesteps3StimulationCurlEx)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	SimulateTimestep(0);
	SimulateTimestep(1);
	SimulateTimestep(2);

	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();
	index_t expectedIndex1 = GetSimulator()->GetIndexToCoordinate(x, y, z);
	index_t expectedIndex2 = GetSimulator()->GetIndexToCoordinate(x, y, z + 1);
	index_t expectedIndex3 = GetSimulator()->GetIndexToCoordinate(x, y, z - 1);
	index_t expectedIndex4 = GetSimulator()->GetIndexToCoordinate(x, y + 1, z);
	index_t expectedIndex5 = GetSimulator()->GetIndexToCoordinate(x, y - 1, z);//


	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float exValue = *degreesOfFreedom->GetElectricVoltageXComponent(index);
				if (index != expectedIndex1 && index != expectedIndex2 && index != expectedIndex3 && index != expectedIndex4 && index != expectedIndex5)
				{
					EXPECT_FLOAT_EQ(exValue, 0.f) << "Index: " << index;
				}
				else
				{
					EXPECT_NE(exValue, 0.f) << "Index: " << index;
				}
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestExSourceTimesteps3StimulationCurlEy)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();

	index_t expectedIndex1 = GetSimulator()->GetIndexToCoordinate(x+1, y, z) ;
	index_t expectedIndex2 = GetSimulator()->GetIndexToCoordinate(x, y, z);
	index_t expectedIndex3 = GetSimulator()->GetIndexToCoordinate(x+1, y - 1, z) ;
	index_t expectedIndex4 = GetSimulator()->GetIndexToCoordinate(x, y - 1, z);
	
	SimulateTimestep(0);
	SimulateTimestep(1);
	SimulateTimestep(2);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float eyValue = *degreesOfFreedom->GetElectricVoltageYComponent(index);
				if (index != expectedIndex1 && index != expectedIndex2 && index != expectedIndex3 && index != expectedIndex4)
				{
					EXPECT_FLOAT_EQ(eyValue, 0.f) << "Index: " << index;
				}
				else
				{
					EXPECT_NE(eyValue, 0.f) << "Index: " << index;
				}
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestExSourceTimesteps3StimulationCurlEz)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();

	index_t expectedIndex1 = GetSimulator()->GetIndexToCoordinate(x+1, y, z);
	index_t expectedIndex2 = GetSimulator()->GetIndexToCoordinate(x, y, z);
	index_t expectedIndex3 = GetSimulator()->GetIndexToCoordinate(x, y, z - 1);
	index_t expectedIndex4 = GetSimulator()->GetIndexToCoordinate(x+1, y, z - 1) ;

	SimulateTimestep(0);
	SimulateTimestep(1);
	SimulateTimestep(2);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float ezValue = *degreesOfFreedom->GetElectricVoltageZComponent(index);
				if (index != expectedIndex1 && index != expectedIndex2 && index != expectedIndex3 && index != expectedIndex4)
				{
					EXPECT_FLOAT_EQ(ezValue, 0.f) << "Index: " << index;
				}
				else
				{
					EXPECT_NE(ezValue, 0.f) << "Index: " << index;
				}
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestExSourceTimesteps3StimulationCurlHx)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);

	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();
	std::list<index_t> indices = {
		//Belong to one curl and are equal-> Curl = 0
		//GetSimulator()->GetIndexToCoordinate(x +1, y, z),
		//GetSimulator()->GetIndexToCoordinate(x +1, y, z-1),

		//Belong to one curl and are equal-> Curl = 0
		//GetSimulator()->GetIndexToCoordinate(x , y, z),
		//GetSimulator()->GetIndexToCoordinate(x , y, z -1),

		//Belong to one curl and are equal-> Curl = 0
		//GetSimulator()->GetIndexToCoordinate(x+1 , y-1, z),
		//GetSimulator()->GetIndexToCoordinate(x+1 , y-1, z-1),

		//Belong to one curl and are equal-> Curl = 0
		//GetSimulator()->GetIndexToCoordinate(x, y-1, z),
		//GetSimulator()->GetIndexToCoordinate(x, y-1, z-1),
	};

	SimulateTimestep(0);
	SimulateTimestep(1);
	SimulateTimestep(2);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float hxValue = *degreesOfFreedom->GetMagneticVoltageXComponent(index);
				if (std::find(indices.begin(), indices.end(), index) != indices.end())
				{
					EXPECT_NE(hxValue, 0.f) << "Index: " << index;
				}
				else
				{
					EXPECT_LE(std::fabs(hxValue), FLT_EPSILON);
					//EXPECT_FLOAT_EQ(hxValue, 0.f) << "Index: " << index;
				}
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestExSourceTimesteps3StimulationCurlHy)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();


	std::list<index_t> indices = {
		GetSimulator()->GetIndexToCoordinate(x, y, z),
		GetSimulator()->GetIndexToCoordinate(x, y, z - 1),
		GetSimulator()->GetIndexToCoordinate(x, y, z - 2),
		GetSimulator()->GetIndexToCoordinate(x, y - 1, z - 1),
		GetSimulator()->GetIndexToCoordinate(x - 1, y, z - 1),
		GetSimulator()->GetIndexToCoordinate(x + 1, y, z - 1),
		GetSimulator()->GetIndexToCoordinate(x, y + 1, z - 1),
		GetSimulator()->GetIndexToCoordinate(x, y - 1, z),
		GetSimulator()->GetIndexToCoordinate(x - 1, y, z),
		GetSimulator()->GetIndexToCoordinate(x + 1, y, z),
		GetSimulator()->GetIndexToCoordinate(x , y + 1, z),
		GetSimulator()->GetIndexToCoordinate(x , y, z + 1)
	};
	

	SimulateTimestep(0);
	SimulateTimestep(1);
	SimulateTimestep(2);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float hyValue = *degreesOfFreedom->GetMagneticVoltageYComponent(index);
				if (std::find(indices.begin(),indices.end(),index) != indices.end())
				{
					EXPECT_NE(hyValue, 0.f) << "Index: " << index;
				}
				else
				{
					EXPECT_FLOAT_EQ(hyValue, 0.f) << "Index: " << index;
				}
				index++;
			}
		}
	}
}

TEST_P(FixtureSimulation3D, TestExSourceTimesteps3StimulationCurlHz)
{
	Alignment alignment = GetParam();
	CreateAirCubeExCentredHardwiredSolver(alignment);
	index_t x = GetSourceCoordinateX();
	index_t y = GetSourceCoordinateY();
	index_t z = GetSourceCoordinateZ();

	std::list<index_t> indices = {
	GetSimulator()->GetIndexToCoordinate(x, y - 1, z + 1),
	GetSimulator()->GetIndexToCoordinate(x, y, z + 1),
	GetSimulator()->GetIndexToCoordinate(x, y, z),
	GetSimulator()->GetIndexToCoordinate(x, y + 1, z),
	GetSimulator()->GetIndexToCoordinate(x, y - 2, z),
	GetSimulator()->GetIndexToCoordinate(x, y - 1, z),
	GetSimulator()->GetIndexToCoordinate(x, y - 1, z - 1),
	GetSimulator()->GetIndexToCoordinate(x, y , z - 1),
	GetSimulator()->GetIndexToCoordinate(x + 1, y , z),
	GetSimulator()->GetIndexToCoordinate(x - 1, y , z),
	GetSimulator()->GetIndexToCoordinate(x + 1, y - 1 , z),
	GetSimulator()->GetIndexToCoordinate(x - 1, y - 1 , z)
	};
	

	SimulateTimestep(0);
	SimulateTimestep(1);
	SimulateTimestep(2);

	auto degreesOfFreedom = GetSimulator()->GetDegreesOfFreedom();

	for (index_t z = 0; z < GetDegreesOfFreedomInZ(); z++)
	{
		for (index_t y = 0; y < GetDegreesOfFreedomInY(); y++)
		{
			index_t index = GetSimulator()->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < GetDegreesOfFreedomInX(); x++)
			{
				float hzValue = *degreesOfFreedom->GetMagneticVoltageZComponent(index);
				if (std::find(indices.begin(), indices.end(), index) != indices.end())
				{
					EXPECT_NE(hzValue, 0.f) << "Index: " << index;
				}
				else
				{
					EXPECT_FLOAT_EQ(hzValue, 0.f) << "Index: " << index;
				}
				index++;
			}
		}
	}
}