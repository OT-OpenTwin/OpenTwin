/*
 *  modelComponent.h
 *
 *  Created on: 08/02/2021
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2021, OpenTwin
 */

#pragma once

 // OpenTwin header
#include "OTCore/ServiceBase.h" // Base class
#include "OTCore/JSON.h"
#include "OTServiceFoundation/FoundationAPIExport.h"

#include "EntityInformation.h"
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
			ModelComponent(const ot::ServiceBase& _serviceInfo, ApplicationBase* _application);
			virtual ~ModelComponent();

			UID createEntityUID(void);

			// Convenience wrapper
			EntityResultText *addResultTextEntity(const std::string &name, const std::string &text);
			void facetAndStoreGeometryEntity(EntityGeometry *entityGeom, bool forceVisible);

			void importTableFile(const std::string &itemName);
		
			bool sendMessage(bool _queue, JsonDocument& _doc, std::string& _response);

			void clearNewEntityList(void);
			void addNewTopologyEntity(UID entityID, UID entityVersion, bool forceVisible);
			void addNewDataEntity(UID entityID, UID entityVersion, UID entityParentID);
			void storeNewEntities(const std::string &description, bool askForBranchCreation = true, bool storeModel = true);

			void loadMaterialInformation();
			std::string getCurrentMaterialName(EntityPropertiesEntityList* material);

		private:
			
			ApplicationBase * m_application = nullptr;

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

			std::map<ot::UID, std::string> materialIDToNameMap;
			std::map<std::string, ot::UID> materialNameToIDMap;
		};
	} // namespace components
} // namespace ot