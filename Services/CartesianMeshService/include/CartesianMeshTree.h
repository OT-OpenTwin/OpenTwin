// @otlicense
// File: CartesianMeshTree.h
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

#include <vector>
#include <ctime>
#include <map>
#include <list>

#include "Geometry.h"

class EntityGeometry;


class CartesianMeshTree
{
public:
	CartesianMeshTree() {}
	virtual ~CartesianMeshTree() { }

	void renderTriangle(float nodes[3][3], float xmin, float xmax, float ymin, float ymax, float zmin, float zmax,
						std::vector<char> &shapeFill, size_t my, size_t mz,
						std::vector<double> &linesX, std::vector<double> &linesY, std::vector<double> &linesZ,
						unsigned int ixmin, unsigned int ixmax, unsigned int iymin, unsigned int iymax, unsigned int izmin, unsigned int izmax, float delta,
						std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>> *triangleInCellInformation, Geometry::Triangle &triangle, EntityGeometry *geometryEntity);

private:
	bool triangleIntersects(float nodes[3][3], float xmin, float xmax, float ymin, float ymax, float zmin, float zmax,
							std::vector<double> &linesX, std::vector<double> &linesY, std::vector<double> &linesZ,
							unsigned int ixmin, unsigned int ixmax, unsigned int iymin, unsigned int iymax, unsigned int izmin, unsigned int izmax, float delta);
};

