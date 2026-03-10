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
	colorRampData.clear();

	vtkNew<vtkLookupTable> lut;
	lut->SetNumberOfTableValues(scalingData->GetColourResolution());
	lut->SetHueRange(.667, 0.0);
	lut->SetAlphaRange(1., 1.);
	lut->IndexedLookupOff();
	lut->SetVectorModeToMagnitude();

	double minVal = 0.0, maxVal = 0.0;
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

	extractColorRampFromLookupTable(lut);

	mapper->SetLookupTable(lut);
}

void VtkDriverWithScaling::extractColorRampFromLookupTable(vtkLookupTable* lut)
{
	double range[2];
	lut->GetTableRange(range);

	vtkIdType n = lut->GetNumberOfTableValues();
	int scale = lut->GetScale();

	std::vector<double> values;
	std::vector<double> colorR;
	std::vector<double> colorG;
	std::vector<double> colorB;

	values.reserve(n + 1);
	colorR.reserve(n);
	colorG.reserve(n);
	colorB.reserve(n);

	for (vtkIdType i = 0; i < n; ++i)
	{
		double rgba[4];
		lut->GetTableValue(i, rgba);

		double start, end;

		if (scale == VTK_SCALE_LOG10)
		{
			double logMin = log10(range[0]);
			double logMax = log10(range[1]);

			double logStart = logMin + (double(i) / n) * (logMax - logMin);
			double logEnd = logMin + (double(i + 1) / n) * (logMax - logMin);

			start = pow(10.0, logStart);
			end = pow(10.0, logEnd);
		}
		else
		{
			start = range[0] + (double(i) / n) * (range[1] - range[0]);
			end = range[0] + (double(i + 1) / n) * (range[1] - range[0]);
		}

		colorR.push_back(rgba[0]);
		colorG.push_back(rgba[1]);
		colorB.push_back(rgba[2]);

		values.push_back(start);
		if (i == n - 1)
		{
			// This is the last value, so we also need to store the end of the range
			values.push_back(end);
		}
	}

	// Now we build information about the colorramp
	std::string labelText = "Header";

	ot::JsonDocument colorRampDoc;
	colorRampDoc.AddMember("Label", ot::JsonString(labelText, colorRampDoc.GetAllocator()), colorRampDoc.GetAllocator());
	colorRampDoc.AddMember("NumberValues", n, colorRampDoc.GetAllocator());
	colorRampDoc.AddMember("Values", ot::JsonArray(values, colorRampDoc.GetAllocator()), colorRampDoc.GetAllocator());
	colorRampDoc.AddMember("ColorR", ot::JsonArray(colorR, colorRampDoc.GetAllocator()), colorRampDoc.GetAllocator());
	colorRampDoc.AddMember("ColorG", ot::JsonArray(colorG, colorRampDoc.GetAllocator()), colorRampDoc.GetAllocator());
	colorRampDoc.AddMember("ColorB", ot::JsonArray(colorB, colorRampDoc.GetAllocator()), colorRampDoc.GetAllocator());

	colorRampData = colorRampDoc.toJson();
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

void VtkDriverWithScaling::getColorRampData(std::string& _colorRampData)
{
	_colorRampData = colorRampData;
}
