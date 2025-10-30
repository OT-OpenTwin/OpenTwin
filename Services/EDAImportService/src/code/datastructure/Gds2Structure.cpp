// @otlicense
// File: Gds2Structure.cpp
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

#include "Gds2Structure.h"

Gds2Structure::Gds2Structure(std::string name, std::vector<MyPolygon> polygons) : name(name), polygons(polygons), structureReferences({}) {}

Gds2Structure::Gds2Structure(std::string name) : name(name), polygons({}), structureReferences({}) {}

Gds2Structure::Gds2Structure(): name(""), polygons({}), structureReferences({}) {}

std::string Gds2Structure::getName(){ return name; }

std::vector<MyPolygon> Gds2Structure::getPolygons(){ return polygons; }

std::vector<StructRef> Gds2Structure::getStructRef(){ return structureReferences; }

std::vector<Gds2Path> Gds2Structure::getPaths(){ return paths; }

void Gds2Structure::setStructRef(std::vector<StructRef> structRef) { structureReferences = structRef; }

void Gds2Structure::setPolygons(std::vector<MyPolygon> polygons) { this->polygons = polygons; }

void Gds2Structure::setPaths(std::vector<Gds2Path> paths) { this->paths = paths; }

void Gds2Structure::addStuctRef(StructRef newStrRef){
	structureReferences.push_back(newStrRef);
}
