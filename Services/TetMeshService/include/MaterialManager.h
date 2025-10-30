// @otlicense
// File: MaterialManager.h
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

#include "Properties.h"
#include "OldTreeIcon.h"

#include <list>
#include <string>
#include <map>

class Application;
class EntityGeometry;
class EntityMaterial;

class MaterialManager
{
public:
	MaterialManager(Application *app) : application(app) {};
	~MaterialManager() {};

	void loadNecessaryMaterials(std::list<EntityGeometry *> geometryEntities, Properties &properties);
	EntityMaterial *getMaterial(const std::string &name);

private:
	Application *application;

	std::map<std::string, EntityMaterial*> materialMap;
};