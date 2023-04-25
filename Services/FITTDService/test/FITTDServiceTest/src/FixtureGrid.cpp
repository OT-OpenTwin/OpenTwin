#include "FixtureGrid.h"
#include "Grid/Grid.h"
#include "SystemDependencies/SystemProperties.h"

void FixtureGrid::DeleteGrid()
{
	if (_grid != nullptr)
	{
		delete _grid;
		_grid = nullptr;
	}
}

int FixtureGrid::GetIntrinsicVectorLength(Alignment activeAlignment) const
{
	if (activeAlignment == AVX512)
	{
		return 16;
	}
	else if (activeAlignment == AVX2)
	{
		return 8;
	}
	else
	{
		return 1;
	}
}

void FixtureGrid::CreateAlignedGridSP(Alignment alignment, int dof)
{
	DeleteGrid();
	_grid = new Grid<float>(dof, dof, dof, alignment);
}

const bool FixtureGrid::AllignmentIsSupported(Alignment alignment)
{
	auto properties = SystemProperties::GetInstance();
	if (alignment == AVX512)
	{
		return properties.CPUSupportsAVX512();
	}
	if (alignment == AVX2)
	{
		return properties.CPUSupportsAVX2();
	}
	else
	{
		return true;
	}


}
