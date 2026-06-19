// @otlicense
// File: FixtureGrid.cpp
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
