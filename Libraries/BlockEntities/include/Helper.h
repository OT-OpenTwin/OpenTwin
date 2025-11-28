// @otlicense
// File: Helper.h
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

// OpenTwin header
#include "OTCore/CoreTypes.h"

// Service header
#include "EntityBlockConnection.h"

// std header
#include <map>

//! @brief Provides helper and utility functions shared between DataProcessingService and CircuitSimulatorService.
class __declspec(dllexport) Helper {
public:
	
	//! @brief Builds a mapping from block UIDs to lists of attached connection entity IDs.
	//! @param _blockConnectionMap Map of block UIDs to EntityBlockConnection shared pointers.
	//!  @param _allEntitiesByBlockID Map of block UIDs to EntityBlock shared pointers.
	//! @return Map associating each participating block UID with its attached connection entity IDs.
	static std::map<ot::UID, ot::UIDList> buildMap(const std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _blockConnectionMap, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allEntitiesByBlockID);
};

