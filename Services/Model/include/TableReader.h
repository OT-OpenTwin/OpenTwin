// @otlicense
// File: TableReader.h
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

#include "EntityCallbackBase.h"

#include <string>
#include <list>

class Model;
class EntityObserver;
class ModelState;

class TableReader
{
public:
	TableReader();
	virtual ~TableReader();

	void setModel(Model *mdl) { model = mdl; }

	std::string readFromFile(const std::string fileName, const std::string &itemName, EntityObserver *obs, ModelState *ms, const ot::EntityCallbackBase& _callbackInfo);

private:
	bool readNextLine(std::ifstream &file, std::string &line);
	std::list<std::string> separateLine(std::string &line, char separator);
	void trimString(std::string &str);

	Model *model;
};
