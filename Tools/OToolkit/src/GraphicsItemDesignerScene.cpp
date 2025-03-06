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
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/GraphicsEllipseItem.h"

// Qt header
#include <QtWidgets/qgraphicssceneevent.h>

GraphicsItemDesignerScene::GraphicsItemDesignerScene(GraphicsItemDesignerView* _view)
	: ot::GraphicsScene(QRectF(0., 0., 300., 200.), _view), m_view(_view), m_mode(NoMode), m_itemSize(300, 200)
{
	_view->setGraphicsScene(this);

	this->setGridStep(10);
	this->setWideGridLineCounter(10);
	this->setGridFlags(ot::Grid::ShowNormalLines | ot::Grid::ShowWideLines);
	this->setGridSnapMode(ot::Grid::SnapTopLeft);
}

GraphicsItemDesignerScene::~GraphicsItemDesignerScene() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void GraphicsItemDesignerScene::enablePickingMode(void) {
	if (m_mode == PointPickingMode) return;
	
	m_mode = PointPickingMode;
}

void GraphicsItemDesignerScene::disablePickingMode(void) {
	if (m_mode != PointPickingMode) return;
		
	m_mode = NoMode;
}

void GraphicsItemDesignerScene::setItemSize(const QSizeF& _size) {
	m_itemSize = _size;
	m_view->setGraphicsSceneRect(QRect(0., 0., m_itemSize.width(), m_itemSize.height()));
}

QImage GraphicsItemDesignerScene::exportAsImage(const GraphicsItemDesignerImageExportConfig& _exportConfig) {
	QRectF oldSceneRect = m_view->sceneRect();

	QRectF itmRect = this->itemsBoundingRect();
	itmRect = itmRect.marginsAdded(_exportConfig.getMargins());
	if (itmRect.width() <= 0 || itmRect.height() <= 0) {
		OT_LOG_W("Nothing to export");
		return QImage();
	}

	ot::Grid::GridFlags oldGridFlags = this->getGridFlags();

	if (_exportConfig.getShowGrid()) {
		this->setGridFlags(ot::Grid::ShowNormalLines | ot::Grid::ShowWideLines);
	}
	else {
		this->setGridFlags(ot::Grid::NoGridFlags);
	}

	m_view->setGraphicsSceneRect(itmRect);

	QImage image(itmRect.size().toSize(), QImage::Format_ARGB32);
	image.fill(Qt::transparent);

	QPainter painter(&image);
	this->render(&painter, QRectF(), itmRect);

	m_view->setGraphicsSceneRect(oldSceneRect);

	this->setGridFlags(oldGridFlags);

	return image;
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
	_painter->fillRect(_rect, cs.getValue(ot::ColorStyleValueEntry::WidgetDisabledBackground).toBrush());
	_painter->fillRect(_rect.intersected(QRectF(QPointF(0., 0.), m_itemSize)), cs.getValue(ot::ColorStyleValueEntry::WidgetBackground).toBrush());

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
