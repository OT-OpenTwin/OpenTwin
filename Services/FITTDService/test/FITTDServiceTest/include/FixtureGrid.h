// @otlicense

#pragma once
#include <gtest/gtest.h>
#include <tuple>

#include "Grid/Grid.h"
#include "SystemDependencies/Allignments.h"

typedef std::tuple<Alignment, int> gridSettings;

class FixtureGrid : public testing::TestWithParam<gridSettings>
{
private:
	Grid<float> * _grid = nullptr;
	index_t _dofX = 0;
	index_t _dofY = 0;
	index_t _dofZ = 0;

	void DeleteGrid();
public:
	~FixtureGrid() {
		DeleteGrid();
	};

	Grid<float> * GetGrid3DSP() { 
		return  _grid;
	};

	int GetIntrinsicVectorLength(Alignment activeAlignment) const;
	

	void CreateAlignedGridSP(Alignment alignment, int dof);

	const bool AllignmentIsSupported(Alignment alignment);
	//ToDo: alignof
};

