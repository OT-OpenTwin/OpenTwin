// @otlicense
// File: Gds2Structure.h
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

#ifndef GDS2STRUCTURE_H_
#define GDS2STRUCTURE_H_

#include <string>
#include <vector>
#include "Polygon.h"
#include "StructRef.h"
#include "Gds2Path.h"




// This class represents a gds2 structure whith all its polygons and structure references
// WARNING: only the boundary type is tracked
class Gds2Structure {
private:
	std::string name;
	std::vector<MyPolygon> polygons;
	std::vector<StructRef> structureReferences;
	std::vector<Gds2Path> paths;
public:
	Gds2Structure(std::string name, std::vector<MyPolygon> polygons);
	Gds2Structure(std::string name);
	Gds2Structure();

	std::string getName();
	std::vector<MyPolygon> getPolygons();
	std::vector<StructRef> getStructRef();
	std::vector<Gds2Path> getPaths();

	void setStructRef(std::vector<StructRef> structRef);
	void setPolygons(std::vector<MyPolygon> polygons);
	void setPaths(std::vector<Gds2Path> paths);
	void addStuctRef(StructRef newStrRef);
};


#endif
