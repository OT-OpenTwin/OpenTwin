// @otlicense
// File: Point.h
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
#include "SystemDependencies/SystemDependentDefines.h"

class Point3D
{
public:
	Point3D(index_t x, index_t y, index_t z)
		: _coordinateX(x), _coordinateY(y), _coordinateZ(z) {};
	
	Point3D(const Point3D &other)
		: _coordinateX(other.GetCoordinateX()), _coordinateY(other.GetCoordinateY()), _coordinateZ(other.GetCoordinateZ()) {};

	const index_t GetCoordinateX() const { return _coordinateX; }
	const index_t GetCoordinateY() const { return _coordinateY; }
	const index_t GetCoordinateZ() const { return _coordinateZ; }

	std::string Print() { return "(" + std::to_string(_coordinateX) + "," + std::to_string(_coordinateY) + "," + std::to_string(_coordinateZ) +")"; }
private:
	index_t _coordinateX;
	index_t _coordinateY;
	index_t _coordinateZ;
};
