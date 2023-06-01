//! @file BlockConnectorManager.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditor/BlockEditorTypes.h"
#include "OTBlockEditor/BlockPaintJob.h"

// std header
#include <list>

namespace ot {

	class BlockLayer;
	class BlockConnector;
	class BlockGraphicsItemGroup;

	class BlockConnectorManager : public ot::BlockPaintJob {
	public:
		BlockConnectorManager(BlockLayer * _layer) : m_layer(_layer) {};
		virtual ~BlockConnectorManager() {};

		BlockLayer* getBlockLayer(void) { return m_layer; };

		//! @brief Add the provided connector the the manager according to the provided flags
		//! @param _connector The connector to add
		//! @param _flags Connector location flags
		virtual bool addConnector(BlockConnector* _connector, const BlockConnectorLocationFlags& _flags) = 0;

		//! @brief Return all connectors that are owned by this manager
		virtual std::list<BlockConnector*> getAllConnectors(void) const = 0;

	private:
		BlockLayer* m_layer;

		BlockConnectorManager(void) = delete;
		BlockConnectorManager(const BlockConnectorManager&) = delete;
		BlockConnectorManager& operator = (const BlockConnectorManager&) = delete;
	};

	class BorderLayoutBlockConnectorManager : public BlockConnectorManager {
	public:
		BorderLayoutBlockConnectorManager(BlockLayer* _layer);
		virtual ~BorderLayoutBlockConnectorManager();

		//! @brief Add the provided connector the the manager according to the provided flags
		//! @param _connector The connector to add
		//! @param _flags Connector location flags
		virtual bool addConnector(BlockConnector* _connector, const BlockConnectorLocationFlags& _flags) override;
		
		//! @brief Return all connectors that are owned by this manager
		virtual std::list<BlockConnector*> getAllConnectors(void) const override;

		virtual QueueResultFlags runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) override;

	private:
		std::list<ot::BlockConnector*> m_top; //! @brief Connectors at the top of the box
		std::list<ot::BlockConnector*> m_right; //! @brief Connectors at the right of the box
		std::list<ot::BlockConnector*> m_bottom; //! @brief Connectors at the bottom of the box
		std::list<ot::BlockConnector*> m_left; //! @brief Connectors at the left of the box

		BorderLayoutBlockConnectorManager(const BorderLayoutBlockConnectorManager&) = delete;
		BorderLayoutBlockConnectorManager& operator = (const BorderLayoutBlockConnectorManager&) = delete;
	};

}