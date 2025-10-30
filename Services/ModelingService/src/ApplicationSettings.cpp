// @otlicense
// File: ApplicationSettings.cpp
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