// @otlicense
// File: Polygon.h
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

#ifndef MYPOLYGON_H_
#define MYPOLYGON_H_

#include <vector>




class MyPolygon{
private: 
		unsigned int layer;
		std::vector<std::pair<int, int>> coordinates;

public:
	MyPolygon(unsigned int l, std::vector<std::pair<int, int>> c);
	MyPolygon();
	
	void setLayer(unsigned int layer);

	unsigned int getLayer();
	std::vector<std::pair<int, int>> getCoordinates();
	void setCoordinates(std::vector<std::pair<int, int>> coords);
};

#endif // !POLYGON_H_
