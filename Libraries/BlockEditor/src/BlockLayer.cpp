//! @file BlockLayer.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockLayer.h"
#include "OTBlockEditor/DefaultBlock.h"
#include "OTBlockEditor/BlockConnectorManager.h"

ot::BlockLayer::BlockLayer(ot::DefaultBlock* _block, BlockConnectorManager* _connectorManager) : BlockPaintJob(NoDelete), m_block(_block), m_connectorManager(_connectorManager), m_orientation(ot::OrientCenter) {

};

ot::BlockLayer::~BlockLayer() {
	if (m_connectorManager) delete m_connectorManager;
}

ot::BlockLayer::QueueResultFlags ot::BlockLayer::runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) {
	paintLayer(_arg->rect(), _arg->painter(), _arg->styleOption(), _arg->widget());
	
	// If a connector manager is set, queue it
	if (m_connectorManager) {
		_queue->queue(m_connectorManager, _arg);
	}

	return Ok;
}

QSizeF ot::BlockLayer::calculateSize(void) const {
	return applyLimits(QSizeF(60., 40.));
}



void ot::BlockLayer::setConnectorManger(BlockConnectorManager* _connectorManager) {
	if (m_connectorManager) delete m_connectorManager;
	m_connectorManager = _connectorManager;
}
