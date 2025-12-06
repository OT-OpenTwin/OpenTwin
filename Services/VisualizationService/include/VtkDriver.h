// @otlicense
// File: VtkDriver.h
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
#include <ctime>

#include "OTCore/CoreTypes.h"

class EntityVis2D3D;
class DataSourceManagerItem;

class VtkDriver
{
public:
	VtkDriver();
	virtual ~VtkDriver();

	virtual void setProperties(EntityVis2D3D *visEntity) = 0;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem) = 0;

	std::list<ot::UID> GetUpdateTopoEntityID() { return updateTopoEntityID; }
	std::list<ot::UID> GetUpdateTopoEntityVersion() { return updateTopoEntityVersion; }
	std::list<bool> GetUpdateTopoEntityForceVisible() { return updateTopoForceVisibility; }

protected:
	void reportTime(const std::string &message, std::time_t &timer);

	std::list<ot::UID> updateTopoEntityID;
	std::list<ot::UID> updateTopoEntityVersion;
	std::list<bool> updateTopoForceVisibility;
};
