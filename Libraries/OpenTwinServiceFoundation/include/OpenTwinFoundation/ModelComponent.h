/*
 *  modelComponent.h
 *
 *  Created on: 08/02/2021
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2021, OpenTwin
 */

#pragma once

 // OpenTwin header
#include "OpenTwinCore/ServiceBase.h"			// Base class
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinFoundation/FoundationAPIExport.h"
#include "OpenTwinFoundation/EntityInformation.h"

#include "EntityProperties.h"

#include "UniqueUIDGenerator.h"

// C++ header
#include <string>
#include <list>
#include <vector>

class EntityBase;
class EntityResult1D;
class EntityPlot1D;
class ClassFactoryHandler;
class EntityResultText;
class EntityGeometry;

namespace ot {

	class ApplicationBase;

	namespace components {

		class OT_SERVICEFOUNDATION_API_EXPORT ModelComponent : public ServiceBase {
		public:
			ModelComponent(
				const std::string &			_name,
				const std::string &			_type,
				const std::string &			_url,
				serviceID_t					_id,
				ApplicationBase *			_application
			);
			virtual ~ModelComponent();


			// #########################################################################################################

			// Model management

			std::string getCurrentModelVersion(void);
			EntityBase * readEntityFromEntityIDandVersion(UID _entityID, UID _version, ClassFactoryHandler &classFactory);
			UID getCurrentVisualizationModelID(void);

			std::list<std::string> getListOfFolderItems(const std::string & _folder, bool recursive = false);
			void getAvailableMeshes(std::string & _meshFolderName, UID & _meshFolderID, std::string & _meshName, UID & _meshID);
			std::list<UID> getIDsOfFolderItemsOfType(const std::string &_folder, const std::string &_entityClassName, bool recursive);

			// #########################################################################################################

			// Entity management

			void addEntitiesToModel(std::list<UID> & _topologyEntityIDList, std::list<UID> & _topologyEntityVersionList, std::list<bool> & _topologyEntityForceVisible, std::list<UID> & _dataEntityIDList, std::list<UID> & _dataEntityVersionList, std::list<UID> & _dataEntityParentList, const std::string & _changeComment);
			void addEntitiesToModel(std::list<UID> && _topologyEntityIDList, std::list<UID> && _topologyEntityVersionList, std::list<bool> && _topologyEntityForceVisible, std::list<UID> && _dataEntityIDList, std::list<UID> && _dataEntityVersionList, std::list<UID> && _dataEntityParentList, const std::string & _changeComment);
			void addGeometryOperation(UID _newEntityID, UID _newEntityVersion, std::string _newEntityName, std::list<UID> & _dataEntityIDList, std::list<UID> & _dataEntityVersionList, std::list<UID> & _dataEntityParentList, std::list<std::string> & _childrenList, const std::string & _changeComment);
			void deleteEntitiesFromModel(std::list<std::string> & _entityNameList, bool _saveModel = true);
			void getEntityInformation(const std::list<UID> & _entities, std::list<EntityInformation> & _entityInfo);
			void getEntityInformation(const std::list<std::string> & _entities, std::list<EntityInformation> & _entityInfo);
			void getEntityInformation(const std::string & _entity, EntityInformation & _entityInfo);
			void getEntityChildInformation(const std::string & _entity, std::list<EntityInformation> & _entityInfo);
			void getEntityChildInformation(UID _entity, std::list<EntityInformation> & _entityInfo);
			void getSelectedEntityInformation(std::list<EntityInformation> & _entityInfo, const std::string& typeFilter);
			void addPropertiesToEntities(std::list<UID>& _entityList, const std::string& _propertiesJson);
			void getEntityProperties(UID _entity, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties);
			void getEntityProperties(const std::string &entityName, bool _recursive, const std::string& _propertyGroupFilter, std::map<UID, EntityProperties>& _entityProperties);

			void updatePropertyGrid();
			void updateTopologyEntities(ot::UIDList& topologyEntityIDs, ot::UIDList& topologyEntityVersions, const std::string& comment);
			
			// Specific entity management helper functions
			EntityPlot1D *addPlot1DEntity(const std::string &name, const std::string &title, const std::list<std::pair<UID, std::string>> &curves);
			EntityResult1D *addResult1DEntity(const std::string &name, const std::vector<double> &xdata, 
											  const std::vector<double> &ydataRe, const std::vector<double> &ydataIm,
											  const std::string &xlabel, const std::string &xunit,
											  const std::string &ylabel, const std::string &yunit, int colorID, bool visible);
			EntityResultText *addResultTextEntity(const std::string &name, const std::string &text);
			void facetAndStoreGeometryEntity(EntityGeometry *entityGeom, bool forceVisible);

			// #########################################################################################################

			// UID management

			UID createEntityUID(void);

			// #########################################################################################################

			void importTableFile(const std::string &itemName);
			void enableMessageQueueing(bool flag);
			void modelChangeOperationCompleted(const std::string &description);

			std::string sendMessage(bool _queue, OT_rJSON_doc& _doc);

			void clearNewEntityList(void);
			void addNewTopologyEntity(UID entityID, UID entityVersion, bool forceVisible);
			void addNewDataEntity(UID entityID, UID entityVersion, UID entityParentID);
			void storeNewEntities(const std::string &description);

		private:
			
			ApplicationBase *			m_application;

			ModelComponent() = delete;
			ModelComponent(const ModelComponent &) = delete;
			ModelComponent & operator = (const ModelComponent &) = delete;

			DataStorageAPI::UniqueUIDGenerator *uniqueUIDGenerator;
			DataStorageAPI::UniqueUIDGenerator* getUniqueUIDGenerator(void);

			std::list<UID> topologyEntityIDList;
			std::list<UID> topologyEntityVersionList;
			std::list<bool> topologyEntityForceVisible;
			std::list<UID> dataEntityIDList;
			std::list<UID> dataEntityVersionList;
			std::list<UID> dataEntityParentList;
		};
	} // namespace components
} // namespace ot