// @otlicense
// File: STLWriter.h
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
#include <sstream>

#include "OTCore/CoreTypes.h"

class Application;
class EntityFacetData;

class STLWriter
{
public:
	STLWriter(Application *_app, const std::string &_serviceName) : application(_app), serviceName(_serviceName) { };
	virtual ~STLWriter() { };

	void getExportFileContent(std::string& data);

private:
	std::list<ot::UID> getAllGeometryEntities(void);
	void appendSTLData(EntityFacetData* facetEntity, std::stringstream& dataStream);
	void writeFacet(std::stringstream& output, double v1x, double v1y, double v1z, double v2x, double v2y, double v2z, double v3x, double v3y, double v3z, double nx, double ny, double nz);

	Application *application;
	std::string serviceName;
};

