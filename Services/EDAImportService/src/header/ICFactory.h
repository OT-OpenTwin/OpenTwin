// @otlicense
// File: ICFactory.h
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

#ifndef ICFACTORY_H_
#define ICFACTORY_H_


#include <vector>
#include <string>

#include "Polygon.h"
#include "Layer.h"
#include "IC.h"
#include "Gds2Import.h"
#include "Gds2Path.h"

class ICFactory{
private:
	static std::vector<Layer> getLayers(std::vector<MyPolygon>& polygons, std::vector<std::pair<int, std::vector<int>>>& stackUpInfo);
	static std::vector<Layer> layers(std::pair < std::vector<MyPolygon>, std::vector<Gds2Path>> &modelData, std::vector<std::pair<int, std::vector<int>>>& stackUpInfo);
public:
	static IC generateIC(std::string name, std::string filepath, std::string stackUpPath);
};

#endif

