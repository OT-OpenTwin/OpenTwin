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
	axisXColor(255, 0, 0, 255), axisYColor(0, 255, 0, 255), axisZColor(0, 0, 255, 255), axisCenterColor(127, 127, 127, 255), axisCenterCrossLineWidth(3),
	axisCenterCrossDashedLineVisible(true), axisCenterCrossLineAtFront(false),
	geometryHighlightColor(255, 0, 0, 255), geometryEdgeColorMode("Same as geometry"), geometryEdgeColorMode_geom("Same as geometry"), geometryEdgeColorMode_custom("Custom"),
	geometryEdgeColorMode_noColor("No Color"), geometryEdgeColor(255, 0, 0, 255), geometryLightSourceDistance("Medium"),
	viewBackgroundColorAutomatic(true), viewBackgroundColor(255, 255, 255, 255), viewForegroundColor(0, 0, 0, 255), useDisplayLists(false), useVertexBufferObjects(true)
{}

ViewerSettings::~ViewerSettings() {}