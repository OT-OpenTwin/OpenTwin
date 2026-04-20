// @otlicense
// File: PropertiesVisUnstructuredScalarSurface.h
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

#pragma once
#include <string>
#include <map>

class _declspec(dllexport) PropertiesVisUnstructuredScalarSurface
{
public:
	enum class VisualizationType
	{
		Points, Contour2D, UNKNOWN
	};

	const std::string GetNameVisType(void) { return nameVisType; }
	const std::string GetNameMaxPoints(void) { return nameMaxPoints; }
	const std::string GetNamePointScale(void) { return namePointScale; }
	const std::string GetNameShow2DMesh(void) { return nameShow2dMesh; }
	const std::string GetName2DMeshColor(void) { return name2dMeshColor; }
	const std::string GetNameShow2DIsolines(void) { return nameShow2dIsolines; }
	const std::string GetName2DIsolineColor(void) { return name2dIsolineColor; }

	const std::string GetValuePoints(void) { return valuePoints; }
	const std::string GetValueContour2D(void) { return valueContour2D; }

	const VisualizationType GetVisualizationType(const std::string type) const
	{
		VisualizationType returnVal;
		visualizationTypeMapping.find(type) == visualizationTypeMapping.end() ? returnVal = VisualizationType::UNKNOWN : returnVal = visualizationTypeMapping.at(type);
		return returnVal;
	}

private:
	const std::string nameVisType = "Type";
	const std::string nameMaxPoints = "Max. Points";
	const std::string namePointScale = "Point Scale";

	const std::string valuePoints     = "Points";
	const std::string valueContour2D  = "2D Contour";

	const std::string nameShow2dMesh = "Show 2D Mesh";
	const std::string name2dMeshColor = "2D Mesh Color";
	const std::string nameShow2dIsolines = "Show Isolines";
	const std::string name2dIsolineColor = "Isoline Color";

	std::map<std::string, VisualizationType> visualizationTypeMapping { {valuePoints,VisualizationType::Points},{valueContour2D,VisualizationType::Contour2D} };
};
