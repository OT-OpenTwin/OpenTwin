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
#include <list>
#include <map>

#include "OTCore/CoreTypes.h"

class EntityBase;
class EntityGeometry;
class Application;

class STEPCAFControl_Reader;
class TopoDS_Shape;
class TDF_Label;
class Model;

class STLWriter
{
public:
	STLWriter(Application *_app, const std::string &_serviceName) : application(_app), serviceName(_serviceName) { };
	virtual ~STLWriter() { };

	void getExportFileContent(std::string& fileContent, unsigned long long& uncompressedDataLength);

private:
	void createSTLFileContent(std::string& data);

	Application *application;
	std::string serviceName;
};

