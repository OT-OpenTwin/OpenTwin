// @otlicense
// File: VtkDriverUnstructuredScalarSurface.cpp
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

#include "VtkDriverWithScaling.h"

#include "OTModelEntities/EntityVis2D3D.h"

#include <vtkNew.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>

VtkDriverWithScaling::VtkDriverWithScaling() {}

VtkDriverWithScaling::~VtkDriverWithScaling()
{
	if (scalingData != nullptr)
	{
		delete scalingData;
		scalingData = nullptr;
	}
}

void VtkDriverWithScaling::getMinMaxScaling(double& minVal, double& maxVal)
{
	assert(scalingData != nullptr);
	assert(scalarRange != nullptr);

	auto scalingMethod = scalingData->GetScalingMethod();
	if (scalingMethod == ScalingProperties::ScalingMethod::rangeScale)
	{

		minVal = scalingData->GetRangeMin();
		maxVal = scalingData->GetRangeMax();
		if (minVal > maxVal)
		{
			//ToDo: UI display message
			minVal = maxVal;
		}
	}
	else if (scalingMethod == ScalingProperties::ScalingMethod::autoScale)
	{
		if (scalingData->GetGlobalRangeSet())
		{
			minVal = scalingData->GetGlobalMin();
			maxVal = scalingData->GetGlobalMax();
		}
		else
		{
			minVal = scalarRange[0];
			maxVal = scalarRange[1];
		}
	}
	else
	{
		throw std::invalid_argument("Not supported scaling method");
	}
}

void VtkDriverWithScaling::SetColouring(vtkPolyDataMapper* mapper)
{
	vtkNew<vtkLookupTable> lut;
	lut->SetNumberOfTableValues(scalingData->GetColourResolution());
	lut->SetHueRange(.667, 0.0);
	lut->SetAlphaRange(1., 1.);
	lut->IndexedLookupOff();
	lut->SetVectorModeToMagnitude();

	double minVal=0.0, maxVal= 0.0;
	getMinMaxScaling(minVal, maxVal);

	assert(scalingData != nullptr);

	auto scalingFunction = scalingData->GetScalingFunction();
	if (scalingFunction == ScalingProperties::ScalingFunction::linScale)
	{
		lut->SetScaleToLinear();
	}
	else if (scalingFunction == ScalingProperties::ScalingFunction::logScale)
	{
		//Log scaling requires the range to be > 0
		if (minVal < 0)
		{
			minVal = 0;
			//ToDo: Message to the UI!
		}
		if (scalingData->GetRangeMax() < 0)
		{
			maxVal = 0;
			if (maxVal < minVal)
			{
				maxVal = minVal;
			}
			//ToDo: Message to the UI!
		}

		lut->SetScaleToLog10();
	}
	else
	{
		throw std::invalid_argument("Not supported scaling function");
	}
	lut->SetTableRange(minVal, maxVal);
	lut->Build();
	mapper->SetLookupTable(lut);
}

void VtkDriverWithScaling::setProperties(EntityVis2D3D* visEntity)
{
	scalingData = new PropertyBundleDataHandleScaling(visEntity);
}

void VtkDriverWithScaling::DeletePropertyData(void)
{
	if (scalingData != nullptr)
	{
		delete scalingData;
		scalingData = nullptr;
	}
}