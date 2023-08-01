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
	QRectF r = boundingRect();
	OT_LOG_D("Rect on: (" + std::to_string(r.x()) + "; " + std::to_string(r.y()) + ")");

	for (auto l : m_layers) {
		paintQueue.queue(l, new BlockPaintJobArg(
			ot::calculateChildRect(r, l->calculateSize(), l->layerOrientation()),
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

QSizeF ot::DefaultBlock::sizeHint(Qt::SizeHint _which, const QSizeF& _constraint) const {
	switch (_which)
	{
	case Qt::MinimumSize: return calculateSize().expandedTo(_constraint);
	case Qt::PreferredSize: return calculateSize().expandedTo(_constraint);
	case Qt::MaximumSize: return calculateSize().boundedTo(_constraint);
	case Qt::MinimumDescent: return calculateSize().expandedTo(_constraint);
	case Qt::NSizeHints: return calculateSize().expandedTo(_constraint);
	default:
		otAssert(0, "Unknown size hint requested");
		return calculateSize();
		break;
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

void ot::DefaultBlock::finalize(void) {
	setPos(0., 0.);

	for (auto l : m_layers) {
		if (l->getConnectorManager()) {
			l->getConnectorManager()->positionChilds();
		}
	}
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

void ot::DefaultBlock::moveChildsBy(const QPointF& _delta) {
	for (auto l : m_layers) {
		ot::BlockConnectorManager* cm = l->getConnectorManager();
		if (cm) {
			for (auto c : cm->getAllConnectors()) {
				c->moveBy(_delta.x(), _delta.y());
			}
		}
	}
}
