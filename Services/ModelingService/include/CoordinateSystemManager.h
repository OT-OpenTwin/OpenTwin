// @otlicense
// File: CoordinateSystemManager.h
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

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"

class CoordinateSystemManager
{
public:
	CoordinateSystemManager(ot::components::UiComponent *_uiComponent, ot::components::ModelComponent *_modelComponent, ot::serviceID_t _serviceID, const std::string &_serviceName) 
		: uiComponent(_uiComponent), modelComponent(_modelComponent), serviceID(_serviceID), serviceName(_serviceName) {};
	virtual ~CoordinateSystemManager() {}

	void createNew();
	void activateCoordinateSystem(const std::string& csName);
	void setActiveCoordinateSystem(const std::string& csName);

protected:
	std::string createUniqueName(const std::string& name);

private:
	void setActiveFlagForCoordinateSystemEntity(const ot::EntityInformation& entityInfo, bool isActive, std::list<ot::UID> &topologyEntityIDList, std::list<ot::UID> &topologyEntityVersionList, std::list<bool> &topologyEntityForceVisible);

	ot::components::UiComponent* uiComponent;
	ot::components::ModelComponent* modelComponent;
	ot::serviceID_t serviceID;
	std::string serviceName;

	std::string activeCoordinateSystemName;
};
