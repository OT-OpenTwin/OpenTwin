//! @file ModelServiceAPI.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTModelAPI/OTModelAPIExport.h"
#include "EntityBase.h"
#include "EntityInformation.h"
#include "NewModelStateInformation.h"
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
		static void addEntitiesToModel(std::list<UID>& _topologyEntityIDList, std::list<UID>& _topologyEntityVersionList, std::list<bool>& _topologyEntityForceVisible, std::list<UID>& _dataEntityIDList, std::list<UID>& _dataEntityVersionList, std::list<UID>& _dataEntityParentList, const std::string& _changeComment, bool askForBranchCreation = true, bool saveModel = true);
		static void addEntitiesToModel(std::list<UID>&& _topologyEntityIDList, std::list<UID>&& _topologyEntityVersionList, std::list<bool>&& _topologyEntityForceVisible, std::list<UID>&& _dataEntityIDList, std::list<UID>&& _dataEntityVersionList, std::list<UID>&& _dataEntityParentList, const std::string& _changeComment, bool askForBranchCreation = true, bool saveModel = true);
		static void addEntitiesToModel(NewModelStateInformation& _newModelStateInfos, const std::string& _changeComment, bool askForBranchCreation = true, bool saveModel = true);
		static void addGeometryOperation(UID _newEntityID, UID _newEntityVersion, std::string _newEntityName, std::list<UID>& _dataEntityIDList, std::list<UID>& _dataEntityVersionList, std::list<UID>& _dataEntityParentList, std::list<std::string>& _childrenList, const std::string& _changeComment);
		static void deleteEntitiesFromModel(std::list<std::string>& _entityNameList, bool _saveModel = true);
		static bool getEntityInformation(UID _entity, EntityInformation& _entityInfo);
		static bool getEntityInformation(const std::string& _entity, EntityInformation& _entityInfo);
		static void getEntityInformation(const std::list<UID>& _entities, std::list<EntityInformation>& _entityInfo);
		static void getEntityInformation(const std::list<std::string>& _entities, std::list<EntityInformation>& _entityInfo);
		static void getEntityChildInformation(const std::string& _entity, std::list<EntityInformation>& _entityInfo, bool recursive);
		static void getEntityChildInformation(UID _entity, std::list<EntityInformation>& _entityInfo, bool recursive);
		static void getSelectedEntityInformation(std::list<EntityInformation>& _entityInfo, const std::string& typeFilter = "");
		static void addPropertiesToEntities(std::list<UID>& _entityList, const ot::PropertyGridCfg& _configuration);
		static void getEntityProperties(UID _entity, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties);
		static void getEntityProperties(const std::string& entityName, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties);
		static void updateTopologyEntities(ot::UIDList& topologyEntityIDs, ot::UIDList& topologyEntityVersions, const std::string& comment);

		static void updatePropertyGrid();
		
	};
}