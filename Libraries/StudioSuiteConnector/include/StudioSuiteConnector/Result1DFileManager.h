// @otlicense
// File: Result1DFileManager.h
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

#include <string>
#include <list>
#include <map>

#include "StudioSuiteConnector/Result1DRunIDContainer.h"


class Result1DFileManager
{
public:
	Result1DFileManager(const std::string& baseDir, std::list<int>& runIds);
	~Result1DFileManager();

	Result1DRunIDContainer* getRunContainer(int id);
	void clear();


private:
	std::map<int, Result1DRunIDContainer*> runIDToDataMap;
};

