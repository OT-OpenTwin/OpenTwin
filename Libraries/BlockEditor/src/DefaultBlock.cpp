//! @file BlockFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTBlockEditor/DefaultBlock.h"
#include "OTBlockEditor/BlockLayer.h"
#include "OTBlockEditor/BlockPaintJob.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/Queue.h"

ot::DefaultBlock::DefaultBlock() {

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
	BlockPaintJobArg paintArgs(boundingRect(), _painter, _option, _widget);

	// Set this so the queue does not destroy this object after calling a paintjob
	paintArgs.noDeleteByQueue();

	// Add every layer as a paintjob, from bottom to top
	// The layers will queue the connectors if needed
	for (auto l : m_layers) { 
		paintQueue.queue(l, &paintArgs); 
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
