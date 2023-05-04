//! @file BorderLayoutConnectorManager.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/ConnectorManager.h"

// std header
#include "vector"

#define OT_BORDERLAYOUTCONNECTORMANAGERCONFIGURATION_TYPE "BorderLayoutConnectorManager"

namespace ot {

	class BlockConnectorConfiguration;

	class BLOCKEDITORAPI_API_EXPORT BorderLayoutConnectorManager : public ot::ConnectorManager {
	public:
		BorderLayoutConnectorManager() {};
		virtual ~BorderLayoutConnectorManager();

		//! @brief Return the connector manager type
		virtual std::string connectorManagerType(void) const override { return OT_BORDERLAYOUTCONNECTORMANAGERCONFIGURATION_TYPE; };

		//! @brief Add the provided connector to the specified location
		//! @param _connector The connector to add
		//! @param _insertLocation The location to insert the connector to
		virtual void addConnector(ot::BlockConnectorConfiguration* _connector, ConnectorManager::ConnectorLocation _insertLocation) override;

		//! @brief Find the connector with the provided name at the specified location
		//! @param _connectorName The name of the connector
		//! @param _searchLocation The location where to search for the connector (AUTO = anywhere)
		virtual BlockConnectorConfiguration* findConnector(const std::string& _connectorName, ConnectorManager::ConnectorLocation _searchLocation) override;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

	private:
		std::vector<ot::BlockConnectorConfiguration*> m_top;
		std::vector<ot::BlockConnectorConfiguration*> m_left;
		std::vector<ot::BlockConnectorConfiguration*> m_right;
		std::vector<ot::BlockConnectorConfiguration*> m_bottom;
	};

}