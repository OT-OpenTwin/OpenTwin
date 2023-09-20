//! @file GraphicsScene.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qgraphicssceneevent.h>

ot::GraphicsScene::GraphicsScene(GraphicsView* _view) : m_gridSize(10), m_view(_view), m_connectionOrigin(nullptr), m_pathItem(nullptr) {

}

ot::GraphicsScene::~GraphicsScene() {}

void ot::GraphicsScene::drawBackground(QPainter* _painter, const QRectF& _rect)
{
	if (m_gridSize < 1) return QGraphicsScene::drawBackground(_painter, _rect);

	QPen pen;
	pen.setColor(QColor(0, 0, 255));
	_painter->setPen(pen);

	qreal left = (qreal)(int(_rect.left()) - (int(_rect.left()) % m_gridSize));
	qreal top = (qreal)(int(_rect.top()) - (int(_rect.top()) % m_gridSize));
	QVector<QPointF> points;
	for (qreal x = left; x < _rect.right(); x += m_gridSize) {
		for (qreal y = top; y < _rect.bottom(); y += m_gridSize) {
			points.append(QPointF(x, y));
		}
	}
	_painter->drawPoints(points.data(), points.size());
}

void ot::GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) {
	QList<QGraphicsItem*> lst = items(_event->scenePos());
	for (auto itm : lst) {
		ot::GraphicsItem* actualItm = dynamic_cast<ot::GraphicsItem*>(itm);
		OTAssertNullptr(actualItm);
		if (actualItm->graphicsItemFlags() & ot::GraphicsItem::ItemIsConnectable) {
			this->startConnection(actualItm);
			QGraphicsScene::mouseDoubleClickEvent(_event);
			return;
		}
	}

	this->stopConnection();

	QGraphicsScene::mouseDoubleClickEvent(_event);
}

void ot::GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _event) {
	if (m_pathItem) {
		OTAssertNullptr(m_connectionOrigin);
		m_pathItem->setPathPoints(m_connectionOrigin->getQGraphicsItem()->pos() + m_connectionOrigin->getQGraphicsItem()->boundingRect().center(), _event->scenePos());
	}
	QGraphicsScene::mouseMoveEvent(_event);
}

void ot::GraphicsScene::startConnection(ot::GraphicsItem* _item) {
	if (m_connectionOrigin == nullptr) {
		// Start new connection
		m_connectionOrigin = _item;

		// ToDo: add preview line
		m_pathItem = new GraphicsPathItem;
		QPen p;
		p.setColor(QColor(64, 255, 64));
		p.setWidth(1);
		m_pathItem->setPen(p);
		m_pathItem->setPathPoints(
			m_connectionOrigin->getQGraphicsItem()->pos() + m_connectionOrigin->getQGraphicsItem()->boundingRect().center(),
			m_connectionOrigin->getQGraphicsItem()->pos() + m_connectionOrigin->getQGraphicsItem()->boundingRect().center()
		);
		this->addItem(m_pathItem->getQGraphicsItem());
		
		return;
	}
	else {
		if (m_connectionOrigin == _item) {
			stopConnection();
			return;
		}
		OT_LOG_D("New conncetion");
		m_view->addConnection(m_connectionOrigin, _item);
		this->stopConnection();
	}
}

void ot::GraphicsScene::stopConnection(void) {
	// Stop connection
	if (m_pathItem) {
		removeItem(m_pathItem);
		delete m_pathItem;
		m_pathItem = nullptr;
		m_connectionOrigin = nullptr;
	}
}
