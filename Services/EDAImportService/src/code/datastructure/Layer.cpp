// @otlicense
// File: Layer.cpp
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

#include "Layer.h"

Layer::Layer(int id_, std::vector<MyPolygon> poly, float height_, float thickness_)
	: id(id_), polygons(poly), height(height_), thikness(thickness_), paths({}) {}

int Layer::getId() { return id; }

float Layer::getThikness() { return thikness; }

float Layer::getHeight() { return height; }

std::vector<MyPolygon> Layer::getPolygons() { return polygons; }

std::vector<Gds2Path> Layer::getPaths(){return paths;}

void Layer::setPaths(std::vector<Gds2Path> _paths) { paths = _paths; }
