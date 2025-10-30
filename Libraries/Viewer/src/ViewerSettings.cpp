// @otlicense
// File: ViewerSettings.cpp
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

#include "stdafx.h"
#include "ViewerSettings.h"

ViewerSettings * g_instance{ nullptr };

ViewerSettings * ViewerSettings::instance(void) {
	if (g_instance == nullptr) { g_instance = new ViewerSettings; }
	return g_instance;
}

void ViewerSettings::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

ViewerSettings::ViewerSettings()
	: workingPlaneStyle_Grid("Grid only"), workingPlaneStyle_Plane("Plane only"), workingPlaneStyle_PlaneGrid("Plane and grid"), workingPlaneStyle("Grid only"),
	workingPlanePlaneColor(153, 153, 153, 31), workingPlaneGridLineColor(166, 166, 166, 255), workingPlaneGridLineWidth(1), workingPlaneWideGridLineWidth(2), workingPlaneAutoSize(true),
	workingPlaneSize(10), workingPlaneDefaultSize(10), workingPlaneGridResolution(1.f), workingPlaneHighlightEveryStep(5), workingPlaneAutoGridResolution(true),
	dimensionSnapping(true), snapSize(0.5), angleSnapping(true), snapAngle(5.0),
	axisXColor(255, 0, 0, 255), axisYColor(0, 255, 0, 255), axisZColor(0, 0, 255, 255), axisCenterColor(127, 127, 127, 255), axisCenterCrossLineWidth(3),
	axisCenterCrossDashedLineVisible(true), axisCenterCrossLineAtFront(false),
	geometryHighlightColor(255, 0, 0, 255), geometryEdgeColorMode("Same as geometry"), geometryEdgeColorMode_geom("Same as geometry"), geometryEdgeColorMode_custom("Custom"),
	geometryEdgeColorMode_noColor("No Color"), geometryEdgeColor(255, 0, 0, 255), geometryLightSourceDistance("Medium"),
	viewBackgroundColorAutomatic(true), viewBackgroundColor(255, 255, 255, 255), viewForegroundColor(0, 0, 0, 255), useDisplayLists(false), useVertexBufferObjects(true),
	cutplaneDrawSolid(true), cutplaneColorFromObject(true), cutplaneFillColor(255, 0, 0), cutplaneOutlineColor(0, 255, 0), cutplaneOutlineWidth(2.0), cutplaneTexture(true)
{}

ViewerSettings::~ViewerSettings() {}