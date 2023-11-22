//! @file GraphicsScene.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsConnectionPreviewItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qgraphicssceneevent.h>

ot::GraphicsScene::GraphicsScene(GraphicsView* _view)
	: m_gridSize(10), m_view(_view), m_connectionOrigin(nullptr), m_connectionPreview(nullptr), m_connectionPreviewStyle(ot::GraphicsConnectionCfg::DirectLine) 
{
	
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
		if (actualItm) {
			if (actualItm->graphicsItemFlags() & ot::GraphicsItemCfg::ItemIsConnectable) {
				this->startConnection(actualItm);
				QGraphicsScene::mouseDoubleClickEvent(_event);
				return;
			}
		}
	}

	this->stopConnection();

	QGraphicsScene::mouseDoubleClickEvent(_event);
}

void ot::GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _event) {
	if (m_connectionPreview) {
		OTAssertNullptr(m_connectionOrigin);
		m_connectionPreview->setOriginPos(m_connectionOrigin->getQGraphicsItem()->scenePos() + m_connectionOrigin->getQGraphicsItem()->boundingRect().center());
		m_connectionPreview->setDestPos(_event->scenePos());
	}
	QGraphicsScene::mouseMoveEvent(_event);
}

void ot::GraphicsScene::startConnection(ot::GraphicsItem* _item) {
	if (m_connectionOrigin == nullptr) {
		// Start new connection
		m_connectionOrigin = _item;

		m_connectionPreview = new GraphicsConnectionPreviewItem;
		QPen p;
		p.setColor(QColor(64, 64, 255));
		p.setWidth(1);
		m_connectionPreview->setPen(p);
		m_connectionPreview->setConnectionStyle(m_connectionPreviewStyle);
		m_connectionPreview->setOriginPos(m_connectionOrigin->getQGraphicsItem()->scenePos() + m_connectionOrigin->getQGraphicsItem()->boundingRect().center());
		m_connectionPreview->setOriginDir(m_connectionOrigin->connectionDirection());
		m_connectionPreview->setDestPos(m_connectionPreview->originPos());
		m_connectionPreview->setDestDir(ot::inversedConnectionDirection(m_connectionOrigin->connectionDirection()));
		this->addItem(m_connectionPreview);
		
		return;
	}
	else {
		if (m_connectionOrigin == _item) {
			this->stopConnection();
			return;
		}
		OT_LOG_D("New conncetion");
		m_view->requestConnection(m_connectionOrigin->getRootItem()->graphicsItemUid(), m_connectionOrigin->graphicsItemName(), _item->getRootItem()->graphicsItemUid(), _item->graphicsItemName());
		this->stopConnection();
	}
}

void ot::GraphicsScene::stopConnection(void) {
	// Stop connection
	if (m_connectionPreview) {
		this->removeItem(m_connectionPreview);
		delete m_connectionPreview;
		m_connectionPreview = nullptr;
		m_connectionOrigin = nullptr;
	}
}
