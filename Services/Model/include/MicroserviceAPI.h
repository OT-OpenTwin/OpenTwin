// @otlicense
// File: MicroserviceAPI.h
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

#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"

#include "Geometry.h"

#include <list>

namespace MicroserviceAPI
{
	void AddDoubleArrayVectorToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::vector<std::array<double, 3>> &vector);
	void AddDoubleArrayPointerToJsonDoc(ot::JsonDocument &doc, const std::string &name, const double *doubleArray, int size);

	void AddEdgeToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::list<Geometry::Edge> &edges);
	void AddTriangleToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::list<Geometry::Triangle> &triangles);
	void AddNodeToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::vector<Geometry::Node> &nodes);

	std::vector<ot::UID> getVectorFromDocument(ot::JsonDocument &doc, const std::string &itemName);
	
	ot::JsonDocument BuildJsonDocFromAction(const std::string &action);
	ot::JsonDocument BuildJsonDocFromString(std::string json);

}

