#pragma once
#include <string>
#include <list>
#include <vector>

#include "OTServiceFoundation/FoundationAPIExport.h"
#include "OTServiceFoundation/EntityInformation.h"
#include "OTCore/Logger.h"
#include "EntityBase.h"

namespace ot {
	class OT_SERVICEFOUNDATION_API_EXPORT ModelServiceAPI
	{

	public:
		ModelServiceAPI(const std::string& thisServiceURL, const std::string& modelServiceURL)
			: m_thisServiceURL(thisServiceURL), m_modelServiceURL(modelServiceURL) {}
		ModelServiceAPI() : m_thisServiceURL(""), m_modelServiceURL("") {};
		// Model management
		std::string getCurrentModelVersion(void);
		EntityBase* readEntityFromEntityIDandVersion(UID _entityID, UID _version, ClassFactoryHandler& classFactory);
		UID getCurrentVisualizationModelID(void);
		std::list<std::string> getListOfFolderItems(const std::string& _folder, bool recursive = false);
		void getAvailableMeshes(std::string& _meshFolderName, UID& _meshFolderID, std::string& _meshName, UID& _meshID);
		std::list<UID> getIDsOfFolderItemsOfType(const std::string& _folder, const std::string& _entityClassName, bool recursive);
		void enableMessageQueueing(bool flag);
		void modelChangeOperationCompleted(const std::string& description);

		// Entity management
		void addEntitiesToModel(std::list<UID>& _topologyEntityIDList, std::list<UID>& _topologyEntityVersionList, std::list<bool>& _topologyEntityForceVisible, std::list<UID>& _dataEntityIDList, std::list<UID>& _dataEntityVersionList, std::list<UID>& _dataEntityParentList, const std::string& _changeComment, bool askForBranchCreation = true, bool saveModel = true);
		void addEntitiesToModel(std::list<UID>&& _topologyEntityIDList, std::list<UID>&& _topologyEntityVersionList, std::list<bool>&& _topologyEntityForceVisible, std::list<UID>&& _dataEntityIDList, std::list<UID>&& _dataEntityVersionList, std::list<UID>&& _dataEntityParentList, const std::string& _changeComment, bool askForBranchCreation = true, bool saveModel = true);
		void addGeometryOperation(UID _newEntityID, UID _newEntityVersion, std::string _newEntityName, std::list<UID>& _dataEntityIDList, std::list<UID>& _dataEntityVersionList, std::list<UID>& _dataEntityParentList, std::list<std::string>& _childrenList, const std::string& _changeComment);
		void deleteEntitiesFromModel(std::list<std::string>& _entityNameList, bool _saveModel = true);
		void getEntityInformation(const std::list<UID>& _entities, std::list<EntityInformation>& _entityInfo);
		void getEntityInformation(const std::list<std::string>& _entities, std::list<EntityInformation>& _entityInfo);
		bool getEntityInformation(const std::string& _entity, EntityInformation& _entityInfo);
		void getEntityChildInformation(const std::string& _entity, std::list<EntityInformation>& _entityInfo, bool recursive);
		void getEntityChildInformation(UID _entity, std::list<EntityInformation>& _entityInfo, bool recursive);
		void getSelectedEntityInformation(std::list<EntityInformation>& _entityInfo, const std::string& typeFilter);
		void addPropertiesToEntities(std::list<UID>& _entityList, const ot::PropertyGridCfg& _configuration);
		void getEntityProperties(UID _entity, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties);
		void getEntityProperties(const std::string& entityName, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties);

		void updatePropertyGrid();
		void updateTopologyEntities(ot::UIDList& topologyEntityIDs, ot::UIDList& topologyEntityVersions, const std::string& comment);		


	protected:
		std::string m_modelServiceURL;
		std::string m_thisServiceURL;

		virtual const std::string& getModelServiceURL() { return m_modelServiceURL; }
		virtual const std::string& getThisServiceURL() { return m_thisServiceURL; }
	};
}