//! @file GraphicsScene.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/GraphicsConnectionPreviewItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qgraphicssceneevent.h>

ot::GraphicsScene::GraphicsScene(GraphicsView* _view)
	: m_gridSize(10), m_gridDoubleEvery(10), m_view(_view), m_connectionOrigin(nullptr), m_connectionPreview(nullptr),
	m_connectionPreviewStyle(ot::GraphicsConnectionCfg::DirectLine), m_ignoreEvents(false), m_mouseIsPressed(false)
{
	this->connect(this, &GraphicsScene::selectionChanged, this, &GraphicsScene::slotSelectionChanged);
}

ot::GraphicsScene::~GraphicsScene() {}

//! @todo Pen width configurable
void ot::GraphicsScene::drawBackground(QPainter* _painter, const QRectF& _rect)
{
	// Use QGraphicsScene method if no grid is set
	if (m_gridSize < 1) return QGraphicsScene::drawBackground(_painter, _rect);

	if (_rect != m_view->mapToScene(m_view->viewport()->rect()).boundingRect()) {
		m_view->update();
		return;
	}

	// Get the current transformation matrix of the painter
	QTransform transform = _painter->transform();

	// Calculate the scaling factor from the transformation matrix
	qreal scaleX = qSqrt(transform.m11() * transform.m11() + transform.m21() * transform.m21());
	qreal scaleY = qSqrt(transform.m22() * transform.m22() + transform.m12() * transform.m12());

	qreal scale = qMax(scaleX, scaleY);

	qreal scaledLineWidth = .5 / scale; // Adjust the line width based on the scale

	// Setup pen
	QPen pen;
	pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(OT_COLORSTYLE_VALUE_ControlsBorderColor).brush());
	pen.setWidthF(scaledLineWidth);

	QPen penWide = pen;
	penWide.setWidthF(scaledLineWidth * 3.);

	QPen penCenter = penWide;
	penCenter.setWidthF(scaledLineWidth * 5.);

	_painter->setPen(pen);
	
	qreal newGridSize = m_gridSize;

	// Calculate grid resolution
	while ((_rect.height() / newGridSize) > 100) {
		newGridSize *= 10.;
	}
	while ((_rect.height() / newGridSize) < 5) {
		newGridSize /= 10.;
	}
	while ((_rect.width() / newGridSize) > 100) {
		newGridSize *= 10.;
	}
	while ((_rect.width() / newGridSize) < 5) {
		newGridSize /= 10.;
	}
	
	int ct = 0;
	for (qreal x = newGridSize; x >= _rect.left(); x -= newGridSize) {
		ct++;
		if (x > _rect.right()) continue;
		if (m_gridDoubleEvery > 1 && (ct % m_gridDoubleEvery) == 0) {
			_painter->setPen(penWide);
			_painter->drawLine(QPointF(x, _rect.top()), QPointF(x, _rect.bottom()));
			_painter->setPen(pen);
			ct = 0;
		}
		else {
			_painter->drawLine(QPointF(x, _rect.top()), QPointF(x, _rect.bottom()));
		}
	}
	ct = 0;
	for (qreal x = newGridSize; x <= _rect.right(); x += newGridSize) {
		ct++;
		if (x < _rect.left()) continue;
		if (m_gridDoubleEvery > 1 && (ct % m_gridDoubleEvery) == 0) {
			_painter->setPen(penWide);
			_painter->drawLine(QPointF(x, _rect.top()), QPointF(x, _rect.bottom()));
			_painter->setPen(pen);
			ct = 0;
		}
		else {
			_painter->drawLine(QPointF(x, _rect.top()), QPointF(x, _rect.bottom()));
		}
	}
	ct = 0;
	for (qreal y = newGridSize; y >= _rect.top(); y -= newGridSize) {
		ct++;
		if (y > _rect.bottom()) continue;
		if (m_gridDoubleEvery > 1 && (ct % m_gridDoubleEvery) == 0) {
			_painter->setPen(penWide);
			_painter->drawLine(QPointF(_rect.left(), y), QPointF(_rect.right(), y));
			_painter->setPen(pen);
			ct = 0;
		}
		else {
			_painter->drawLine(QPointF(_rect.left(), y), QPointF(_rect.right(), y));
		}
	}
	ct = 0;
	for (qreal y = newGridSize; y <= _rect.bottom(); y += newGridSize) {
		ct++;
		if (y < _rect.top()) continue;
		if (m_gridDoubleEvery > 1 && (ct % m_gridDoubleEvery) == 0) {
			_painter->setPen(penWide);
			_painter->drawLine(QPointF(_rect.left(), y), QPointF(_rect.right(), y));
			_painter->setPen(pen);
			ct = 0;
		}
		else {
			_painter->drawLine(QPointF(_rect.left(), y), QPointF(_rect.right(), y));
		}
	}
	
	_painter->setPen(penCenter);
	_painter->drawLine(QPointF(0, _rect.top()), QPointF(0, _rect.top()));
	_painter->drawLine(QPointF(0, _rect.top()), QPointF(0, _rect.top()));
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

void ot::GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	if (_event->button() == Qt::LeftButton) {
		m_lastSelection = this->selectedItems();
		m_mouseIsPressed = true;
	}
	QGraphicsScene::mousePressEvent(_event);
}

void ot::GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	QGraphicsScene::mouseReleaseEvent(_event);

	if (_event->button() == Qt::LeftButton) {
		m_mouseIsPressed = false;
	}
}

void ot::GraphicsScene::slotSelectionChanged(void) {
	if (m_ignoreEvents || m_mouseIsPressed) return;
	this->handleSelectionChanged();
}

void ot::GraphicsScene::handleSelectionChanged(void) {
	QList<QGraphicsItem*> tmp = m_lastSelection;
	m_lastSelection = this->selectedItems();
	if (tmp.size() != m_lastSelection.size()) {
		Q_EMIT selectionChangeFinished();
	}
	else {
		for (QGraphicsItem* itm : m_lastSelection) {
			bool found = false;
			for (QGraphicsItem* itmCheck : tmp) {
				if (itm == itmCheck) {
					found = true;
					break;
				}
			}
			if (!found) {
				Q_EMIT selectionChangeFinished();
				return;
			}
		}
	}
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
