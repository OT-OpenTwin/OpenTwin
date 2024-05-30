//! @file GraphicsItemDesignerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/GraphicsEllipseItem.h"

// Qt header
#include <QtWidgets/qgraphicssceneevent.h>

GraphicsItemDesignerScene::GraphicsItemDesignerScene(GraphicsItemDesignerView* _view)
	: ot::GraphicsScene(QRectF(0., 0., 300., 200.), _view), m_view(_view), m_cursorItem(nullptr), m_mode(NoMode), m_itemSize(300, 200)
{
	this->setGridFlags(ot::Grid::ShowNormalLines | ot::Grid::ShowWideLines);
	this->setGridSnapMode(ot::Grid::SnapTopLeft);
}

GraphicsItemDesignerScene::~GraphicsItemDesignerScene() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void GraphicsItemDesignerScene::enablePickingMode(void) {
	if (m_mode == PointPickingMode) return;
	if (m_cursorItem) delete m_cursorItem;
	m_cursorItem = nullptr;

	m_mode = PointPickingMode;
	return;

	m_cursorItem = new ot::GraphicsEllipseItem;
	m_cursorItem->setRadius(1, 1);
	m_cursorItem->setBrush(QBrush(QColor(200, 200, 0)));
	m_cursorItem->setPen(QPen(Qt::NoPen));
	m_cursorItem->setGraphicsScene(this);
	m_cursorItem->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemHasNoFeedback);
	m_cursorItem->setFlag(QGraphicsItem::ItemIsSelectable, false);

	this->addItem(m_cursorItem);
}

void GraphicsItemDesignerScene::disablePickingMode(void) {
	if (m_mode != PointPickingMode) return;
	if (m_cursorItem) {
		this->removeItem(m_cursorItem);
		delete m_cursorItem;
		m_cursorItem = nullptr;
	}
	
	m_mode = NoMode;
}

void GraphicsItemDesignerScene::setItemSize(const QSizeF& _size) {
	m_itemSize = _size;
	m_view->setSceneRect(QRect(0., 0., m_itemSize.width(), m_itemSize.height()));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Event handler

void GraphicsItemDesignerScene::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	ot::GraphicsScene::mousePressEvent(_event);

	if (m_mode == PointPickingMode) {
		m_view->fwdPointSelected(_event->scenePos());
	}
}

void GraphicsItemDesignerScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _event) {
	ot::GraphicsScene::mouseMoveEvent(_event);
	
	m_view->fwdPositionChanged(_event->scenePos());

	if (m_cursorItem) { 
		m_cursorItem->prepareGraphicsItemGeometryChange();
		m_cursorItem->setPos(_event->scenePos() - QPointF(m_cursorItem->radiusX(), m_cursorItem->radiusY()));
	}

	if (m_mode == PointPickingMode) {
		QGraphicsItem* item = itemAt(_event->scenePos(), QTransform());
		if (item) {
			this->constrainItemToScene(item);
		}
	}
}

void GraphicsItemDesignerScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	ot::GraphicsScene::mouseReleaseEvent(_event);

	if (m_mode == PointPickingMode) {
		QGraphicsItem* item = itemAt(_event->scenePos(), QTransform());
		if (item) {
			this->constrainItemToScene(item);
		}
	}
}

void GraphicsItemDesignerScene::drawBackground(QPainter* _painter, const QRectF& _rect) {
	if (_rect != m_view->mapToScene(m_view->viewport()->rect()).boundingRect()) {
		m_view->update();
		return;
	}

	const ot::ColorStyle& cs = ot::GlobalColorStyle::instance().getCurrentStyle();
	_painter->fillRect(_rect, cs.getValue(OT_COLORSTYLE_VALUE_ControlsDisabledBackground).brush());
	_painter->fillRect(_rect.intersected(QRectF(QPointF(0., 0.), m_itemSize)), cs.getValue(OT_COLORSTYLE_VALUE_ControlsBackground).brush());

	this->drawGrid(_painter, _rect.intersected(QRectF(QPointF(0., 0.), m_itemSize)));
}

void GraphicsItemDesignerScene::constrainItemToScene(QGraphicsItem* _item)
{
	QRectF itemRect = _item->sceneBoundingRect();
	QPointF newPos = _item->pos();

	if (itemRect.left() < 0) {
		newPos.setX(0);
	}
	else if (itemRect.right() > m_itemSize.width()) {
		newPos.setX(newPos.x() - (newPos.x() - m_itemSize.height()));
	}

	if (itemRect.top() < 0) {
		newPos.setY(0);
	}
	else if (itemRect.bottom() > m_itemSize.height()) {
		newPos.setY(newPos.y() - (newPos.y() - m_itemSize.height()));
	}
	
	if (newPos != _item->pos()) _item->setPos(newPos);

	itemRect = _item->sceneBoundingRect();
	if (itemRect.left() < 0 || itemRect.top() < 0) {
		OT_LOG_E("Item is too big");
	}
}