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
	workingPlanePlaneColor(0.6f, 0.6f, 0.6f, 0.12f), workingPlaneGridLineColor(0.65f, 0.65f, 0.65f, 1.0f), workingPlaneGridLineWidth(1), workingPlaneWideGridLineWidth(2), workingPlaneAutoSize(true),
	workingPlaneSize(10), workingPlaneDefaultSize(10), workingPlaneGridResolution(1.f), workingPlaneHighlightEveryStep(5), workingPlaneAutoGridResolution(true),
	axisXColor(1.f, 0.f, 0.f, 1.f), axisYColor(0.f, 1.f, 0.f, 1.f), axisZColor(0.f, 0.f, 1.f, 1.f), axisCenterColor(0.5f, 0.5f, 0.5f, 1.f), axisCenterCrossLineWidth(3),
	axisCenterCrossDashedLineVisible(true), axisCenterCrossLineAtFront(false),
	geometryHighlightColor(1.f, 0.f, 0.f, 1.f), geometryEdgeColorMode("Same as geometry"), geometryEdgeColorMode_geom("Same as geometry"), geometryEdgeColorMode_custom("Custom"),
	geometryEdgeColorMode_noColor("No Color"), geometryEdgeColor(1.f, 0.f, 0.f, 1.f), geometryLightSourceDistance("Medium"),
	viewBackgroundColorAutomatic(true), viewBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f), viewForegroundColor(0.0f, 0.0f, 0.0f, 0.0f), useDisplayLists(true), useVertexBufferObjects(false)
{}

ViewerSettings::~ViewerSettings() {}