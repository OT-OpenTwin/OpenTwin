// @otlicense

#include "ApplicationSettings.h"

ApplicationSettings * g_settings{ nullptr };

ApplicationSettings * ApplicationSettings::instance(void) {
	if (g_settings == nullptr) { g_settings = new ApplicationSettings; }
	return g_settings;
}

ApplicationSettings::ApplicationSettings()
	: geometryDefaultColor(153, 153, 242), geometryDefaultName("New item")
{
	geometryNamingMode = geometryNamingModeGeometryBased;
}