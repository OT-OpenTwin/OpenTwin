// @otlicense
// File: ICFactory.cpp
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

#include "ICFactory.h"

#include <map>
#include <iostream>

#define LAYER 0
#define HEIGHT 1
#define THIKNESS 2


std::vector<Layer> ICFactory::getLayers(std::vector<MyPolygon>& polygons, std::vector<std::pair<int, std::vector<int>>>& stackUpInfo) {
	std::map<int, std::vector<MyPolygon>> layerMap = {};

	// polygons get mapped to the corresponding layer
	for (auto& polygon : polygons) {
		layerMap[polygon.getLayer()].push_back(polygon);
	}

	// create layer objects
	std::vector<Layer> layers = {};

	if (stackUpInfo.size() != layerMap.size() || stackUpInfo[0].second.size() != 3) {
		for (auto& layer : layerMap) {

		std::cout << "Layer : " << layer.first << "; " << "Number of Polygons: " << layer.second.size() << std::endl;

		Layer newLayer(layer.first, layer.second, 150, 50);
		layers.push_back(newLayer);
		}
	}
	else {
		for (auto& pair : stackUpInfo) {
			Layer newLayer(pair.second[0], layerMap[pair.second[0]], pair.second[1], pair.second[2]);
			layers.push_back(newLayer);
		}
	}
	

	return layers;
}
std::vector<Layer> ICFactory::layers(std::pair < std::vector<MyPolygon>, std::vector<Gds2Path>>& modelData, std::vector<std::pair<int, std::vector<int>>>& stackUpInfo) {
	std::map<int, std::vector<MyPolygon>> layerMap = {};
	std::map<int, std::vector<Gds2Path>> pathLayerMap = {};

	// polygons get mapped to the corresponding layer
	for (auto& polygon : modelData.first) {
		layerMap[polygon.getLayer()].push_back(polygon);
	}

	for (auto& path : modelData.second) {
		pathLayerMap[path.getLayer()].push_back(path);
	}

	//create layer objects
	std::vector<Layer> layers = {};
	std::vector<Gds2Path> paths = {};

	if (stackUpInfo.size() != layerMap.size() || stackUpInfo[0].second.size() != 3) {
		for (auto& layer : layerMap) {

			std::cout << "Layer : " << layer.first << "; " << "Number of Polygons: " << layer.second.size() << std::endl;

			Layer newLayer(layer.first, layer.second, 50, 150);
			layers.push_back(newLayer);
		}

		for (auto& pathLayer : pathLayerMap) {
			if (layerMap.count(pathLayer.first) == 1) {
				for (auto& layer : layers) {
					if (layer.getId() == pathLayer.first) {
						layer.setPaths(pathLayer.second);
					}
				}
			}
			else {
				Layer newLayer(pathLayer.first, {}, 50, 100);
				newLayer.setPaths(pathLayer.second);
				layers.push_back(newLayer);
			}
		}
	}
	else {
		for (auto& pair : stackUpInfo) {
			Layer newLayer(pair.second[LAYER], layerMap[pair.second[LAYER]], pair.second[HEIGHT], pair.second[THIKNESS]);
			newLayer.setPaths(pathLayerMap[pair.second[LAYER]]);
			layers.push_back(newLayer);
		}
	}


	return layers;
}

IC ICFactory::generateIC(std::string name, std::string filepath, std::string stackUpPath) {
	Gds2Import import(filepath);
	std::vector<std::pair<int, std::vector<int>>> stackUpInfo = import.readStackUp(stackUpPath);
	std::vector<MyPolygon> polygons = import.getPolygons();
	std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>> modelData = import.modelData();
	std::vector<Layer> layer = layers(modelData, stackUpInfo);

	IC new_IC(name, layer);

	return new_IC;
}
