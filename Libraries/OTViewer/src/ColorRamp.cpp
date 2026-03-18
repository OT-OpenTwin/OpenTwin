// @otlicense
// File: ColorRamp.cpp
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
 
#include "OTViewer/ColorRamp.h"
#include "OTCore/JSON/JSONDocument.h"
#include "OTCore/JSON/JSONObject.h"
#include "OTCore/JSON/JSONArray.h"
#include "OTCore/JSON/JSONHelper.h"

void ColorRamp::loadFromString(const std::string& colorRampData)
{
	ot::JsonDocument colorRamp;
	colorRamp.fromJson(colorRampData);

	label = ot::json::getString(colorRamp, "Label");
	values = ot::json::getDoubleVector(colorRamp, "Values");
	colorR = ot::json::getDoubleVector(colorRamp, "ColorR");
	colorG = ot::json::getDoubleVector(colorRamp, "ColorG");
	colorB = ot::json::getDoubleVector(colorRamp, "ColorB");
}

bool ColorRamp::operator==(const ColorRamp& other) const
{
	if (label != other.label) return false;

	if (values.size() != other.values.size()) return false;
	if (values.size() < 2) return true;

	assert(colorR.size() == values.size() - 1);
	assert(colorG.size() == values.size() - 1);
	assert(colorB.size() == values.size() - 1);

	double valueTol = (values[values.size() - 1] - values[0]) * 1e-6;
	double colorTol = 1e-4;

	for (size_t i = 0; i < values.size(); i++)
	{
		if (fabs(values[i] - other.values[i]) > valueTol) return false;
	}

	for (size_t i = 0; i < colorR.size(); i++)
	{
		if (fabs(colorR[i] - other.colorR[i]) > colorTol) return false;
		if (fabs(colorG[i] - other.colorG[i]) > colorTol) return false;
		if (fabs(colorB[i] - other.colorB[i]) > colorTol) return false;
	}

	// The two color ramps seem to be the same within the given tolerances
	return true;
}


