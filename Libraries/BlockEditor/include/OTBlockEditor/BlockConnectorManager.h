//! @file BlockConnectorManager.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorTypes.h"

namespace ot {

	class BlockLayer;
	class BlockConnector;

	class BlockConnectorManager {
	public:
		enum ConnectorLocation {

		};

		BlockConnectorManager(BlockLayer * _layer) : m_layer(_layer) {};
		virtual ~BlockConnectorManager() {};

		BlockLayer* getBlockLayer(void) { return m_layer; };

		//! @brief Add the provided connector the the manager according to the provided flags
		virtual bool addConnector(BlockConnector* _connector, const BlockConnectorLocationFlags& _flags) = 0;

	private:
		BlockLayer* m_layer;

		BlockConnectorManager(void) = delete;
		BlockConnectorManager(const BlockConnectorManager&) = delete;
		BlockConnectorManager& operator = (const BlockConnectorManager&) = delete;
	};

	class DefaultBlockConnectorManager : public BlockConnectorManager {
	public:
		DefaultBlockConnectorManager(BlockLayer* _layer);
		virtual ~DefaultBlockConnectorManager();

		virtual bool addConnector(BlockConnector* _connector, const BlockConnectorLocationFlags& _flags) override;

	private:

		DefaultBlockConnectorManager(const DefaultBlockConnectorManager&) = delete;
		DefaultBlockConnectorManager& operator = (const DefaultBlockConnectorManager&) = delete;
	};

}