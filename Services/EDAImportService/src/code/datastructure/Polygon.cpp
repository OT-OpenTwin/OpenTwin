// @otlicense
// File: Polygon.cpp
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

#include "Polygon.h"

MyPolygon::MyPolygon(unsigned int l, std::vector<std::pair<int, int>> c) : layer(l), coordinates(c){}

MyPolygon::MyPolygon() {
	layer = 0;
	coordinates = {};
}

void MyPolygon::setCoordinates(std::vector<std::pair<int, int>> coords){ coordinates = coords; }

void MyPolygon::setLayer(unsigned int layer) { this->layer = layer; }

unsigned int MyPolygon::getLayer() { return layer; }

std::vector<std::pair<int, int>> MyPolygon::getCoordinates() { return coordinates; }
