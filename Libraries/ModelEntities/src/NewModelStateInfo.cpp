// @otlicense
// File: NewModelStateInfo.cpp
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

// OpenTwin header
#include "NewModelStateInfo.h"

void ot::NewModelStateInfo::addTopologyEntity(ot::UID _entityID, ot::UID _entityVersion, bool _forceVisible) {
	OTAssert(_entityID != 0, "Entity ID must not be zero");
	OTAssert(_entityID != ot::invalidUID, "Invalid entity ID");
	OTAssert(_entityVersion != 0, "Entity version must not be zero");
	OTAssert(_entityVersion != ot::invalidUID, "Invalid entity version");

	m_topologyEntityIDs.push_back(_entityID);
	m_topologyEntityVersions.push_back(_entityVersion);
	m_topologyForceVisible.push_back(_forceVisible);
}

void ot::NewModelStateInfo::addTopologyEntity(const EntityBase& _entityTopology, bool _forceVisible) {
	OTAssert(_entityTopology.getEntityType() == EntityBase::entityType::TOPOLOGY, "Entity must be of type TOPOLOGY");
	OTAssert(_entityTopology.getEntityID() != 0, "Entity ID must not be zero");
	OTAssert(_entityTopology.getEntityID() != ot::invalidUID, "Invalid entity ID");
	OTAssert(_entityTopology.getEntityStorageVersion() != 0, "Entity version must not be zero");
	OTAssert(_entityTopology.getEntityStorageVersion() != ot::invalidUID, "Invalid entity version");

	m_topologyEntityIDs.push_back(_entityTopology.getEntityID());
	m_topologyEntityVersions.push_back(_entityTopology.getEntityStorageVersion());
	m_topologyForceVisible.push_back(_forceVisible);
}

void ot::NewModelStateInfo::addDataEntity(ot::UID _parentTopoEntityID, ot::UID _entityID, ot::UID _entityVersion) {
	OTAssert(_parentTopoEntityID != 0, "Parent topology entity ID must not be zero");
	OTAssert(_parentTopoEntityID != ot::invalidUID, "Invalid parent topology entity ID");
	OTAssert(_entityID != 0, "Entity ID must not be zero");
	OTAssert(_entityID != ot::invalidUID, "Invalid entity ID");
	OTAssert(_entityVersion != 0, "Entity version must not be zero");
	OTAssert(_entityVersion != ot::invalidUID, "Invalid entity version");

	m_dataEntityIDs.push_back(_entityID);
	m_dataEntityVersions.push_back(_entityVersion);
	m_dataEntityParentIDs.push_back(_parentTopoEntityID);
}

void ot::NewModelStateInfo::addDataEntity(ot::UID _parentTopoEntityID, const EntityBase& _entityData) {
	OTAssert(_parentTopoEntityID != 0, "Parent topology entity ID must not be zero");
	OTAssert(_parentTopoEntityID != ot::invalidUID, "Invalid parent topology entity ID");
	OTAssert(_entityData.getEntityID() != 0, "Entity ID must not be zero");
	OTAssert(_entityData.getEntityID() != ot::invalidUID, "Invalid entity ID");
	OTAssert(_entityData.getEntityStorageVersion() != 0, "Entity version must not be zero");
	OTAssert(_entityData.getEntityStorageVersion() != ot::invalidUID, "Invalid entity version");

	m_dataEntityIDs.push_back(_entityData.getEntityID());
	m_dataEntityVersions.push_back(_entityData.getEntityStorageVersion());
	m_dataEntityParentIDs.push_back(_parentTopoEntityID);
}

void ot::NewModelStateInfo::addDataEntity(const EntityBase& _parentTopoEntity, const EntityBase& _entityData) {
	OTAssert(_parentTopoEntity.getEntityType() == EntityBase::entityType::TOPOLOGY, "Parent entity must be of type TOPOLOGY");
	OTAssert(_parentTopoEntity.getEntityID() != 0, "Parent topology entity ID must not be zero");
	OTAssert(_parentTopoEntity.getEntityID() != ot::invalidUID, "Invalid parent topology entity ID");
	OTAssert(_entityData.getEntityID() != 0, "Entity ID must not be zero");
	OTAssert(_entityData.getEntityID() != ot::invalidUID, "Invalid entity ID");
	OTAssert(_entityData.getEntityStorageVersion() != 0, "Entity version must not be zero");
	OTAssert(_entityData.getEntityStorageVersion() != ot::invalidUID, "Invalid entity version");

	m_dataEntityIDs.push_back(_entityData.getEntityID());
	m_dataEntityVersions.push_back(_entityData.getEntityStorageVersion());
	m_dataEntityParentIDs.push_back(_parentTopoEntity.getEntityID());
}

void ot::NewModelStateInfo::addDataEntity(const EntityBase& _parentTopoEntity, ot::UID _entityID, ot::UID _entityVersion) {
	OTAssert(_parentTopoEntity.getEntityType() == EntityBase::entityType::TOPOLOGY, "Parent entity must be of type TOPOLOGY");
	OTAssert(_parentTopoEntity.getEntityID() != 0, "Parent topology entity ID must not be zero");
	OTAssert(_parentTopoEntity.getEntityID() != ot::invalidUID, "Invalid parent topology entity ID");
	OTAssert(_entityID != 0, "Entity ID must not be zero");
	OTAssert(_entityID != ot::invalidUID, "Invalid entity ID");
	OTAssert(_entityVersion != 0, "Entity version must not be zero");
	OTAssert(_entityVersion != ot::invalidUID, "Invalid entity version");

	m_dataEntityIDs.push_back(_entityID);
	m_dataEntityVersions.push_back(_entityVersion);
	m_dataEntityParentIDs.push_back(_parentTopoEntity.getEntityID());
}
