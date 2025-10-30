// @otlicense
// File: ImportInterface.h
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

#ifndef IMPORTINTERDFACE_H_
#define IMPORTINTERDFACE_H_

#include <vector>
#include <string>
#include "Polygon.h"



class ImportInterface{
public:
	virtual std::vector<MyPolygon> getPolygons(std::vector<std::byte> data);
	virtual std::vector<std::byte> readFileData(const std::string& name);
};

#endif

