/*
 *  modelComponent.h
 *
 *  Created on: 08/02/2021
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2021, OpenTwin
 */

#pragma once

 // OpenTwin header
#include "OTCore/ServiceBase.h"			// Base class
#include "OTServiceFoundation/ModelServiceAPI.h"
#include "OTCore/JSON.h"
#include "OTServiceFoundation/FoundationAPIExport.h"
#include "OTServiceFoundation/EntityInformation.h"

#include "EntityProperties.h"

#include "UniqueUIDGenerator.h"

#include "EntityResult1DPlot.h"
#include "EntityResult1DCurve.h"

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

		class OT_SERVICEFOUNDATION_API_EXPORT ModelComponent : public ServiceBase, public ModelServiceAPI {
		public:
			ModelComponent(
				const std::string &			_name,
				const std::string &			_type,
				const std::string &			_url,
				serviceID_t					_id,
				ApplicationBase *			_application
			);
			virtual ~ModelComponent();

			UID createEntityUID(void);

			// Convenience wrapper
			EntityResult1DPlot * addResult1DPlotEntity(const std::string &name, const std::string &title, const std::list<std::pair<UID, std::string>> &curves);
			EntityResult1DCurve * addResult1DCurveEntity(const std::string &name, const std::vector<double> &xdata,
											  const std::vector<double> &ydataRe, const std::vector<double> &ydataIm,
											  const std::string &xlabel, const std::string &xunit,
											  const std::string &ylabel, const std::string &yunit, int colorID, bool visible);
			EntityResultText *addResultTextEntity(const std::string &name, const std::string &text);
			void facetAndStoreGeometryEntity(EntityGeometry *entityGeom, bool forceVisible);

			void importTableFile(const std::string &itemName);
		
			std::string sendMessage(bool _queue, JsonDocument& _doc);

			void clearNewEntityList(void);
			void addNewTopologyEntity(UID entityID, UID entityVersion, bool forceVisible);
			void addNewDataEntity(UID entityID, UID entityVersion, UID entityParentID);
			void storeNewEntities(const std::string &description, bool askForBranchCreation = true);

		private:
			
			ApplicationBase * m_application = nullptr;

			virtual const std::string& getModelServiceURL() override;
			virtual const std::string& getThisServiceURL() override;

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