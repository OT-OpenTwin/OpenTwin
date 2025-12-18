// @otlicense
// File: ModelServiceAPI.h
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
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTModelAPI/OTModelAPIExport.h"
#include "EntityBase.h"
#include "NewModelStateInfo.h"
#include "EntityInformation.h"
// std header
#include <string>
#include <list>
#include <vector>

namespace ot {

	class OT_MODELAPI_API_EXPORT ModelServiceAPI {
		OT_DECL_NOCOPY(ModelServiceAPI)
		OT_DECL_NOMOVE(ModelServiceAPI)
		OT_DECL_NODEFAULT(ModelServiceAPI)
	public:
		// Model management
		
		static std::string getCurrentModelVersion(void);
		static UID getCurrentVisualizationModelID(void);
		static std::list<std::string> getListOfFolderItems(const std::string& _folder, bool recursive = false);
		static void getAvailableMeshes(std::string& _meshFolderName, UID& _meshFolderID, std::string& _meshName, UID& _meshID);
		static void getAvailableScripts(std::string& _meshFolderName, UID& _meshFolderID, std::string& _meshName, UID& _meshID);
		static std::list<UID> getIDsOfFolderItemsOfType(const std::string& _folder, const std::string& _entityClassName, bool recursive);
		static void enableMessageQueueing(bool flag);
		static void modelChangeOperationCompleted(const std::string& description);

		// Entity management

		static void addEntitiesToModel(const std::list<UID>& _topologyEntityIDList, const std::list<UID>& _topologyEntityVersionList, const std::list<bool>& _topologyEntityForceVisible, const std::list<UID>& _dataEntityIDList, const std::list<UID>& _dataEntityVersionList, const std::list<UID>& _dataEntityParentList, const std::string& _changeComment, bool askForBranchCreation = true, bool saveModel = true, bool _considerVisualization = true);
		static void addEntitiesToModel(std::list<UID>&& _topologyEntityIDList, std::list<UID>&& _topologyEntityVersionList, std::list<bool>&& _topologyEntityForceVisible, std::list<UID>&& _dataEntityIDList, std::list<UID>&& _dataEntityVersionList, std::list<UID>&& _dataEntityParentList, const std::string& _changeComment, bool askForBranchCreation = true, bool saveModel = true, bool _considerVisualization = true);
		static void addEntitiesToModel(const NewModelStateInfo& _newModelStateInfos, const std::string& _changeComment, bool _askForBranchCreation = true, bool _saveModel = true, bool _considerVisualization = true);
		
		static void addGeometryOperation(UID _newEntityID, UID _newEntityVersion, std::string _newEntityName, std::list<UID>& _dataEntityIDList, std::list<UID>& _dataEntityVersionList, std::list<UID>& _dataEntityParentList, std::list<std::string>& _childrenList, const std::string& _changeComment);
		
		static void deleteEntityFromModel(UID _entityID, bool _saveModel = true);
		static void deleteEntityFromModel(const std::string& _entityName, bool _saveModel = true);
		static void deleteEntitiesFromModel(const std::list<std::string>& _entityNameList, bool _saveModel = true);
		static void deleteEntitiesFromModel(const UIDList& _entityIDList, bool _saveModel = true);

		//! @brief Will fill the provided EntityInformation object with information about the requested entity. Only works for topology entities. 
		//! Requesting information about data entities will result in a default EntityInformation object being returned.
		static bool getEntityInformation(UID _entity, EntityInformation& _entityInfo);
		
		//! @brief Will fill the provided EntityInformation object with information about the requested entity. Only works for topology entities.
		//! Requesting information about data entities will result in a default EntityInformation object being returned.
		static bool getEntityInformation(const std::string& _entity, EntityInformation& _entityInfo);
		
		//! @brief Will fill the provided EntityInformation object with information about the requested entity. Only works for topology entities.
		//! Requesting information about data entities will result in a default EntityInformation object being returned.
		static void getEntityInformation(const std::list<UID>& _entities, std::list<EntityInformation>& _entityInfo);
		
		//! @brief Will fill the provided EntityInformation object with information about the requested entity. Only works for topology entities.
		//! Requesting information about data entities will result in a default EntityInformation object being returned.
		static void getEntityInformation(const std::list<std::string>& _entities, std::list<EntityInformation>& _entityInfo);
		
		static void getEntityChildInformation(const std::string& _entity, std::list<EntityInformation>& _entityInfo, bool recursive);
		static void getEntityChildInformation(UID _entity, std::list<EntityInformation>& _entityInfo, bool recursive);
		
		static void getSelectedEntityInformation(std::list<EntityInformation>& _entityInfo, const std::string& typeFilter = "");
		
		static void addPropertiesToEntities(std::list<UID>& _entityList, const ot::PropertyGridCfg& _configuration);
		
		static void getEntityProperties(UID _entity, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties);
		static void getEntityProperties(const std::string& entityName, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties);
		
		static void updateTopologyEntities(const NewModelStateInfo& _updatedEntities, const std::string& _comment, bool _considerVisualization = true);
		static void updateTopologyEntities(const ot::UIDList& topologyEntityIDs, const ot::UIDList& topologyEntityVersions, const std::string& comment, bool _considerVisualization = true);

		static void updatePropertyGrid();
		
	};
}