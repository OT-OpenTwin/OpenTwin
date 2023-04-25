#include "ApplicationSettings.h"

ApplicationSettings * g_settings{ nullptr };

ApplicationSettings * ApplicationSettings::instance(void) {
	if (g_settings == nullptr) { g_settings = new ApplicationSettings; }
	return g_settings;
}

ApplicationSettings::ApplicationSettings()
	: geometryDefaultColor(0.6f, 0.6f, 0.95f, 1.f), geometryDefaultName("New item")
{
	geometryNamingMode = geometryNamingModeGeometryBased;
}