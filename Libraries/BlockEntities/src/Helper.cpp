// @otlicense
// File: Helper.cpp
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


#include "Helper.h"

std::map<ot::UID, ot::UIDList> Helper::buildMap(const std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _blockConnectionMap, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allEntitiesByBlockID) {

    //  Result map: block UID -> list of attached connection entity IDs
	std::map<ot::UID, ot::UIDList> connectionBlockMap;

    //  Iterate over all connections
    for (const auto& connectionByUID : _blockConnectionMap) {
        const std::shared_ptr<EntityBlockConnection>& connection = connectionByUID.second;
        const auto& cfg = connection->getConnectionCfg();

        // Extract origin and destination block UIDs
        ot::UID originUID = cfg.getOriginUid();
        ot::UID destinationUID = cfg.getDestinationUid();

        // Skip connections with invalid UIDs
        if (destinationUID == ot::invalidUID || originUID == ot::invalidUID) {
            continue;
        }
        
        // Check if both blocks exist in entity map
        auto originIt = _allEntitiesByBlockID.find(originUID);
        auto destIt = _allEntitiesByBlockID.find(destinationUID);

        // Add connection ID to both endpoint blocks if valid
        if (originIt != _allEntitiesByBlockID.end()
            && destIt != _allEntitiesByBlockID.end()) {
            connectionBlockMap[originUID].push_back(connection->getEntityID());
            connectionBlockMap[destinationUID].push_back(connection->getEntityID());
        }
	}
	return connectionBlockMap;
}
