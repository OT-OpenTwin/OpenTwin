// @otlicense

#pragma once

#include "OTCore/Color.h"

#include <string>

class ApplicationSettings {
public:
	static ApplicationSettings * instance(void);

	const std::string	geometryNamingModeGeometryBased{ "According to geometry type" };
	const std::string	geometryNamingModeCustom{ "Custom" };

	ot::Color		geometryDefaultColor;
	std::string		geometryNamingMode;
	std::string		geometryDefaultName;

private:

	ApplicationSettings();
	ApplicationSettings(ApplicationSettings&) = delete;
	ApplicationSettings& operator = (ApplicationSettings&) = delete;
};