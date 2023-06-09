//! @file BlockFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/DefaultBlock.h"
#include "OTBlockEditor/BlockLayer.h"
#include "OTBlockEditor/BlockPaintJob.h"
#include "OTBlockEditor/BlockHelper.h"
#include "OTBlockEditor/BlockConnector.h"
#include "OTBlockEditor/BlockConnectorManager.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/Queue.h"

ot::DefaultBlock::DefaultBlock(BlockGraphicsItemGroup* _graphicsItemGroup) : ot::Block(_graphicsItemGroup) {

}

ot::DefaultBlock::~DefaultBlock() {
	for (auto l : m_layers) delete l;
}

qreal ot::DefaultBlock::blockWidth(void) const {
	double w = 0.;
	for (auto l : m_layers) {
		QSizeF sh = l->layerSizeHint();
		if (sh.width() > w) w = sh.width();
	}
	return w;
}

qreal ot::DefaultBlock::blockHeigth(void) const {
	double h = 0.;
	QSizeF sh;
	for (auto l : m_layers) {
		sh = l->layerSizeHint();
		if (sh.height() > h) h = sh.height();
	}
	return h;
}

void ot::DefaultBlock::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) {
	// Ensure we have at least one layer to paint
	if (m_layers.empty()) {
		OT_LOG_WA("No layers provided, skipping block paint");
		return;
	}

	// Create paint job queue and arguments object
	SimpleQueue paintQueue;
	
	// Add every layer as a paintjob, from bottom to top
	// The layers will queue the connectors if needed
	// The arg pointer will be removed my the queue
	for (auto l : m_layers) { 
		paintQueue.queue(l, new BlockPaintJobArg(
			ot::calculateChildRect(boundingRect(), l->layerOrientation(), l->layerSizeHint()).marginsRemoved(l->margins()),
			_painter, 
			_option,
			_widget)
		);
	}

	// Run the paintjob
	if (!paintQueue.executeQueue()) {
		OT_LOG_EA("Failed to execute paintjob queue");
	}

}

void ot::DefaultBlock::addLayer(BlockLayer* _layer) {
	OTAssertNullptr(_layer);

#ifdef _DEBUG
	// Duplicate layer check in debug mode
	for (auto l : m_layers) {
		if (l == _layer) {
			OT_LOG_EA("Duplicate layer provided");
			return;
		}
	}
#endif // _DEBUG
	
	m_layers.push_back(_layer);
}

void ot::DefaultBlock::attachChildsToGroup(BlockGraphicsItemGroup* _gig) {
	for (BlockLayer * layer : m_layers) {
		ot::BlockConnectorManager * connectorManager = layer->getConnectorManager();
		if (connectorManager) {
			for (BlockConnector * connector : connectorManager->getAllConnectors()) {
				_gig->addToGroup(connector);
			}
		}
	}
}
