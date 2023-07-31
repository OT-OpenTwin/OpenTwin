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

// Qt header
#include <QtWidgets/qgraphicsscene.h>

ot::DefaultBlock::DefaultBlock() {

}

ot::DefaultBlock::~DefaultBlock() {
	for (auto l : m_layers) delete l;
}

QSizeF ot::DefaultBlock::calculateSize(void) const {
	QSizeF s(10., 10.);

	// Determine max width and height
	for (auto l : m_layers) { s = s.expandedTo(l->calculateSize()); }

	return s;
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
			ot::calculateChildRect(boundingRect(), l->calculateSize(), l->layerOrientation()),
			_painter,
			_option,
			_widget)
		);
	}

	// Run the paintjob
	if (!paintQueue.executeQueue()) {
		OT_LOG_EA("Failed to execute paintjob queue");
	}

	// Call the base class paint function to handle mouse and move highlights
	ot::Block::paint(_painter, _option, _widget);
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

void ot::DefaultBlock::placeChildsOnScene(QGraphicsScene* _scene) {
	for (auto l : m_layers) {
		ot::BlockConnectorManager * cm = l->getConnectorManager();
		if (cm) {
			for (auto c : cm->getAllConnectors()) {
				_scene->addItem(c);
			}
		}
	}
}
