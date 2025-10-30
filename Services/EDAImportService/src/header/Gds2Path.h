// @otlicense
// File: Gds2Path.h
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

#ifndef GDS2PATH_H_
#define GDS2PATH_H_

#include <vector>
#include <string>
#include "Polygon.h"

class Gds2Path {
private:
	unsigned int width; // width of the path
	unsigned int layer; 
	unsigned int pathType;
	std::vector<std::pair<int, int>> coordinates;

	std::pair<int, int> calcVector(std::pair<int, int>& p1, std::pair<int, int>& p2);
	std::pair<int, int> calcOffset(std::pair<int, int>& p1, std::pair<int, int>& p2);

	


public:
	MyPolygon pathToPolygon();

	unsigned int getWidth();
	unsigned int getLayer();
	unsigned int getPathType();
	std::vector<std::pair<int, int>> getCoordinates();
	void setWidth(int width);
	void setLayer(unsigned int layer);
	void setPathType(unsigned int pathType);
	void setCoordinates(std::vector<std::pair<int, int>> coordinates);

};

#endif

