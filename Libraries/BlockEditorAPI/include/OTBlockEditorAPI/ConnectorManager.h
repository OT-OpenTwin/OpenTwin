//! @file ConnectorManager.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockConfigurationObject.h"
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"

// std header
#include "vector"

#define OT_JSON_MEMBER_ConnectorManagerType "ConnectorManagerType"

namespace ot {

	class BlockConnectorConfiguration;

	class BLOCKEDITORAPI_API_EXPORT ConnectorManager : public ot::BlockConfigurationObject {
	public:
		enum ConnectorLocation {
			TOP,
			LEFT,
			RIGHT,
			BOTTOM,
			AUTO
		};

		ConnectorManager() {};
		virtual ~ConnectorManager() {};

		//! @brief Return the connector manager type
		virtual std::string connectorManagerType(void) const = 0;

		//! @brief Add the provided connector to the specified location
		//! @param _connector The connector to add
		//! @param _insertLocation The location to insert the connector to
		virtual void addConnector(ot::BlockConnectorConfiguration* _connector, ConnectorLocation _insertLocation) = 0;

		//! @brief Find the connector with the provided name at the specified location
		//! @param _connectorName The name of the connector
		//! @param _searchLocation The location where to search for the connector (AUTO = anywhere)
		virtual BlockConnectorConfiguration* findConnector(const std::string& _connectorName, ConnectorLocation _searchLocation) = 0;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;
	};

}