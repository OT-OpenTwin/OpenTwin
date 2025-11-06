// @otlicense
// File: ViewerSettings.h
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

#include "OTCore/Color.h"

#include <string>

class ViewerSettings {
public:
	static ViewerSettings * instance(void);

	static void deleteInstance(void);

	std::string	workingPlaneStyle;
	std::string	workingPlaneStyle_Plane;
	std::string	workingPlaneStyle_Grid;
	std::string	workingPlaneStyle_PlaneGrid;
	ot::Color	workingPlanePlaneColor;
	ot::Color	workingPlaneGridLineColor;
	
	bool		workingPlaneAutoGridResolution;
	int			workingPlaneGridLineWidth;
	int			workingPlaneWideGridLineWidth;
	float		workingPlaneGridResolution;
	int			workingPlaneHighlightEveryStep;

	bool		workingPlaneAutoSize;
	int			workingPlaneDefaultSize;
	int			workingPlaneSize;

	bool		dimensionSnapping;
	double		snapSize;
	bool		angleSnapping;
	double		snapAngle;

	ot::Color	axisXColor;
	ot::Color	axisYColor;
	ot::Color	axisZColor;
	ot::Color	axisCenterColor;
	int			axisCenterCrossLineWidth;
	bool		axisCenterCrossDashedLineVisible;
	bool		axisCenterCrossLineAtFront;

	bool        viewBackgroundColorAutomatic;
	ot::Color	viewBackgroundColor;
	ot::Color	viewForegroundColor;

	// Geometry attributes
	std::string geometryEdgeColorMode;
	std::string geometryEdgeColorMode_geom;
	std::string geometryEdgeColorMode_custom;
	std::string geometryEdgeColorMode_noColor;
	double geometrySelectionTransparency;
	ot::Color geometryHighlightColor;
	double geometryHighlightLineWidth;
	ot::Color geometryEdgeColor;
	std::string geometryLightSourceDistance;

	// Mesh attributes
	ot::Color   meshEdgeColor;

	// Display attributes
	bool useDisplayLists;
	bool useVertexBufferObjects;

	// Cutplane attributes
	bool      cutplaneDrawSolid;
	bool      cutplaneColorFromObject;
	ot::Color cutplaneFillColor;
	ot::Color cutplaneOutlineColor;
	double    cutplaneOutlineWidth;
	bool	  cutplaneTexture;

private:
	ViewerSettings();
	~ViewerSettings();
	ViewerSettings(ViewerSettings&) = delete;
	ViewerSettings& operator = (ViewerSettings&) = delete;

};