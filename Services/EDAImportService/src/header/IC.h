// @otlicense
// File: IC.h
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

#ifndef IC_H_
#define IC_H_

#include <vector>
#include <string>
#include "Layer.h"

class IC{
private:
	std::string name; // name of the ic
	std::vector<Layer> layers; // layer of which the ic is build up

public:
	IC(std::string name_, std::vector<Layer> layers_);

	std::string getName();
	std::vector<Layer> getLayers();

};

#endif

