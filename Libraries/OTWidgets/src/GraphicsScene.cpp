//! @file GraphicsScene.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/GraphicsConnectionPreviewItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qgraphicssceneevent.h>

ot::GraphicsScene::GraphicsScene(GraphicsView* _view)
	: m_view(_view), m_connectionOrigin(nullptr), m_connectionPreview(nullptr),
	m_connectionPreviewShape(ot::GraphicsConnectionCfg::ConnectionShape::DirectLine), m_ignoreEvents(false), m_mouseIsPressed(false),
	m_maxTriggerDistance(0.)
{
	OTAssertNullptr(m_view);
	this->connect(this, &GraphicsScene::selectionChanged, this, &GraphicsScene::slotSelectionChanged);
}

ot::GraphicsScene::GraphicsScene(const QRectF& _sceneRect, GraphicsView* _view)
	: QGraphicsScene(_sceneRect), m_view(_view), m_connectionOrigin(nullptr), m_connectionPreview(nullptr),
	m_connectionPreviewShape(ot::GraphicsConnectionCfg::ConnectionShape::DirectLine), m_ignoreEvents(false), m_mouseIsPressed(false),
	m_maxTriggerDistance(0.)
{
	OTAssertNullptr(m_view);
	this->connect(this, &GraphicsScene::selectionChanged, this, &GraphicsScene::slotSelectionChanged);
}

ot::GraphicsScene::~GraphicsScene() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Connection handling

void ot::GraphicsScene::startConnectionToConnection(ot::GraphicsConnectionItem* _targetedConnection, const Point2DD& _pos) {
	OTAssertNullptr(m_view);
	if (m_view->getGraphicsViewFlags() & GraphicsView::IgnoreConnectionByUser) return;

	// Currently a connection may be used as a destiantion only for other connections
	if (!m_connectionOrigin) {
		return;
	}

	OT_LOG_D("New conncetion to connection");
	m_view->requestConnectionToConnection(m_connectionOrigin->getRootItem()->getGraphicsItemUid(), m_connectionOrigin->getGraphicsItemName(), _targetedConnection->getConfiguration().getUid(), _pos);
	this->stopConnection();
}

void ot::GraphicsScene::startConnection(ot::GraphicsItem* _item) {
	OTAssertNullptr(m_view);
	if (m_view->getGraphicsViewFlags() & GraphicsView::IgnoreConnectionByUser) return;

	if (m_connectionOrigin == nullptr) {
		// Start new connection
		m_connectionOrigin = _item;

		m_connectionPreview = new GraphicsConnectionPreviewItem;
		QPen p;
		p.setColor(QColor(64, 64, 255));
		p.setWidth(1);
		m_connectionPreview->setConnectionShape(m_connectionPreviewShape);
		m_connectionPreview->setOriginPos(m_connectionOrigin->getQGraphicsItem()->scenePos() + m_connectionOrigin->getQGraphicsItem()->boundingRect().center());
		m_connectionPreview->setOriginDir(m_connectionOrigin->getConnectionDirection());
		m_connectionPreview->setDestPos(m_connectionPreview->originPos());
		m_connectionPreview->setDestDir(ot::inversedConnectionDirection(m_connectionOrigin->getConnectionDirection()));
		this->addItem(m_connectionPreview);

		return;
	}
	else {
		if (m_connectionOrigin == _item) {
			this->stopConnection();
			return;
		}
		OT_LOG_D("New conncetion");
		m_view->requestConnection(m_connectionOrigin->getRootItem()->getGraphicsItemUid(), m_connectionOrigin->getGraphicsItemName(), _item->getRootItem()->getGraphicsItemUid(), _item->getGraphicsItemName());
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

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsScene::checkMaxTriggerDistance(double _triggerDistance) {
	if (_triggerDistance > m_maxTriggerDistance) {
		m_maxTriggerDistance = _triggerDistance;
	}
}

void ot::GraphicsScene::checkMaxTriggerDistance(const MarginsD& _triggerDistance) {
	this->checkMaxTriggerDistance(std::max({ _triggerDistance.left(), _triggerDistance.top(), _triggerDistance.right(), _triggerDistance.bottom() }));
}	

QPointF ot::GraphicsScene::snapToGrid(const QPointF& _pos) const {
	return QtFactory::toQPoint(m_grid.snapToGrid(QtFactory::toPoint2D(_pos)));
}

ot::Point2DD ot::GraphicsScene::snapToGrid(const Point2DD& _pos) const {
	return m_grid.snapToGrid(_pos);
}

void ot::GraphicsScene::deselectAll(void) {
	this->blockSignals(true);
	for (QGraphicsItem* itm : this->selectedItems()) {
		itm->setSelected(false);
	}
	this->blockSignals(false);
	Q_EMIT selectionChanged();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

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

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Event handling

void ot::GraphicsScene::moveAllSelectedItems(const Point2DD& _delta) {
	if (_delta.x() == 0. && _delta.y() == 0.) return;
	for (QGraphicsItem* item : this->selectedItems()) {
		GraphicsItem* otItem = dynamic_cast<GraphicsItem*>(item);
		if (otItem) {
			if (otItem->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsMoveable) {
				otItem->setGraphicsItemPos(this->snapToGrid(otItem->getGraphicsItemPos() + _delta));
			}
		}
	}
}

void ot::GraphicsScene::rotateAllSelectedItems(double _relativeAngle) {
	if (_relativeAngle == 0.) return;

	for (QGraphicsItem* item : this->selectedItems()) {
		GraphicsItem* otItem = dynamic_cast<GraphicsItem*>(item);
		if (otItem) {
			if (otItem->getGraphicsItemFlags() & GraphicsItemCfg::ItemUserTransformEnabled) {
				Transform itemTransform = otItem->getGraphicsItemTransform();
				double newAngle = itemTransform.getRotation();
				newAngle += _relativeAngle;
				while (newAngle >= 360.) {
					newAngle -= 360.;
				}
				while (newAngle < 0.) {
					newAngle += 360.;
				}
				itemTransform.setRotation(newAngle);
				otItem->setGraphicsItemTransform(itemTransform);
			}
		}
	}
}

void ot::GraphicsScene::flipAllSelectedItems(Qt::Orientation _flipAxis) {
	for (QGraphicsItem* item : this->selectedItems()) {
		GraphicsItem* otItem = dynamic_cast<GraphicsItem*>(item);
		if (otItem) {
			if (otItem->getGraphicsItemFlags() & GraphicsItemCfg::ItemUserTransformEnabled) {
				Transform itemTransform = otItem->getGraphicsItemTransform();
				if (_flipAxis == Qt::Horizontal) {
					itemTransform.setFlipState(Transform::FlipHorizontally, !(itemTransform.getFlipStateFlags() & Transform::FlipHorizontally));
				}
				else if (_flipAxis == Qt::Vertical) {
					itemTransform.setFlipState(Transform::FlipVertically, !(itemTransform.getFlipStateFlags() & Transform::FlipVertically));
				}
				otItem->setGraphicsItemTransform(itemTransform);
			}
		}
	}
}

void ot::GraphicsScene::itemAboutToBeRemoved(GraphicsItem* _item) {
	if (_item == m_connectionOrigin) {
		m_connectionOrigin = nullptr;
		this->stopConnection();
	}
}

void ot::GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) {
	// Check for a new connection
	QList<QGraphicsItem*> lst;
	if (m_maxTriggerDistance > 0.) {
		QRectF hitRect(
			_event->scenePos().x() - m_maxTriggerDistance,
			_event->scenePos().y() - m_maxTriggerDistance,
			m_maxTriggerDistance * 2.,
			m_maxTriggerDistance * 2.
		);

		lst = items(hitRect);
	}
	else {
		lst = items(_event->scenePos());
	}
	
	qreal minDistance = std::numeric_limits<double>::max();
	GraphicsBase* targetedBase = nullptr;
	bool targetedIsItem = false;

	for (auto itm : lst) {
		ot::GraphicsBase* actualBase = dynamic_cast<ot::GraphicsBase*>(itm);
		/*if (actualItm) {
			if (actualItm->getGraphicsItemFlags() & ot::GraphicsItemCfg::ItemIsConnectable) {
				this->startConnection(actualItm);
				//QGraphicsScene::mouseDoubleClickEvent(_event);
				return;
			}
		}
		*/
		if (actualBase) {
			ot::GraphicsItem* actualItm = dynamic_cast<ot::GraphicsItem*>(itm);

			qreal dist = actualBase->calculateShortestDistanceToPoint(_event->scenePos());
			
			// Item
			if (actualItm) {
				if ((actualItm->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable) && dist >= 0. && ((dist < minDistance && targetedIsItem) || !targetedIsItem)) {
					minDistance = dist;
					targetedBase = actualBase;
					targetedIsItem = true;
				}
			}
			// Connection
			else if (dist >= 0. && dist < minDistance && !targetedIsItem) {
				minDistance = dist;
				targetedBase = actualBase;
			}
		}
	}

	// Check if a base item was found next to the click position.
	if (targetedBase) {
		GraphicsItem* graphicsItem = dynamic_cast<ot::GraphicsItem*>(targetedBase);
		GraphicsConnectionItem* connectionItem = dynamic_cast<GraphicsConnectionItem*>(targetedBase);

		if (graphicsItem) {
			this->startConnection(graphicsItem);
		}
		else if (connectionItem) {
			this->startConnectionToConnection(connectionItem, QtFactory::toPoint2D(_event->scenePos()));
		}
	}
	else {
		this->stopConnection();
	}
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
		this->handleSelectionChanged();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Helper

void ot::GraphicsScene::drawBackground(QPainter* _painter, const QRectF& _rect)
{
	// Use QGraphicsScene method if no grid is set
	if (!m_grid.isGridLinesValid()) {
		QGraphicsScene::drawBackground(_painter, _rect);
	}
	else {
		this->drawGrid(_painter, _rect);
	}
}

void ot::GraphicsScene::drawGrid(QPainter* _painter, const QRectF& _rect) {
	QList<QLineF> normalLines;
	QList<QLineF> wideLines;
	QList<QLineF> centerLines;
	this->calculateGridLines(_rect, normalLines, wideLines, centerLines);

	// Setup pen
	QPen pen = QtFactory::toQPen(m_grid.getGridLineStyle());
	qreal scaledLineWidth = this->calculateScaledGridLineWidth(_painter, pen.widthF());

	pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::Border).brush());
	pen.setWidthF(scaledLineWidth);
	
	if (!normalLines.isEmpty()) {
		_painter->setPen(pen);
		_painter->drawLines(normalLines);
	}

	if (!wideLines.isEmpty()) {
		pen.setWidthF(scaledLineWidth * 3.);
		_painter->setPen(pen);
		_painter->drawLines(wideLines);
	}
	
	if (!centerLines.isEmpty()) {
		pen.setWidthF(scaledLineWidth * 3.);
		pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::WidgetHoverBackground).brush());
		_painter->setPen(pen);
		_painter->drawLines(centerLines);
	}
}

qreal ot::GraphicsScene::calculateScaledGridLineWidth(QPainter* _painter, qreal _normalWidth) const {
	// Get the current transformation matrix of the painter
	QTransform transform = _painter->transform();

	// Calculate the scaling factor from the transformation matrix
	qreal scaleX = qSqrt(transform.m11() * transform.m11() + transform.m21() * transform.m21());
	qreal scaleY = qSqrt(transform.m22() * transform.m22() + transform.m12() * transform.m12());

	qreal scale = qMax(scaleX, scaleY);

	return _normalWidth / scale;
}

void ot::GraphicsScene::calculateGridLines(const QRectF& _painterRect, QList<QLineF>& _normalLines, QList<QLineF>& _wideLines, QList<QLineF>& _centerLines) const {
	if (!m_grid.isGridLinesValid()) return;
	
	// Center line
	if (m_grid.getGridFlags() & Grid::ShowCenterCross) {
		if (0. >= _painterRect.left() && 0. <= _painterRect.right()) {
			_centerLines.push_back(QLineF(0., _painterRect.top(), 0., _painterRect.bottom()));
		}
		if (0. >= _painterRect.top() && 0. <= _painterRect.bottom()) {
			_centerLines.push_back(QLineF(_painterRect.left(), 0., _painterRect.right(), 0.));
		}
	}

	// Scale the grid step size if AutoScaleGrid is set
	

	// Get starting point
	Point2D scaledStepSize = this->calculateScaledGridStepSize(m_view->mapToScene(m_view->viewport()->rect()).boundingRect());
	QPointF startPos = QtFactory::toQPoint(Grid::snapToGrid(QtFactory::toPoint2D(_painterRect.topLeft()), scaledStepSize));

	int lineCounterX = 0;
	int lineCounterY = 0;


	// If wide lines will be displayed the line counter need to be calculated
	if ((m_grid.getGridFlags() & Grid::ShowWideLines) && (m_grid.getWideGridLineCounter().x() > 1 || m_grid.getWideGridLineCounter().y() > 1)) {

		// Calculate initial line counter for X axis
		lineCounterX = (int)(startPos.x() / scaledStepSize.x());
		if (lineCounterX < 0) {
			lineCounterX *= (-1);
		}
		if (startPos.x() <= _painterRect.left()) {
			if (startPos.x() < 0.) {
				lineCounterX--;
			}
			else {
				lineCounterX++;
			}
			startPos.setX(startPos.x() + scaledStepSize.x());
		}

		// Calculate initial line counter for Y axis
		lineCounterY = (int)(startPos.y() / scaledStepSize.y());
		if (lineCounterY < 0) {
			lineCounterY *= (-1);
		}
		if (startPos.y() <= _painterRect.top()) {
			if (startPos.y() < 0.) {
				lineCounterY--;
			}
			else {
				lineCounterY++;
			}
			startPos.setY(startPos.y() + scaledStepSize.y());
		}

		// And now use modulo to get the current counter value in the counter range
		lineCounterX = (lineCounterX % m_grid.getWideGridLineCounter().x());
		lineCounterY = (lineCounterY % m_grid.getWideGridLineCounter().y());
	}

	// Calculate X axis lines
	for (qreal x = startPos.x(); x < _painterRect.right(); x += scaledStepSize.x())
	{
		if (m_grid.getGridFlags() & Grid::ShowNormalLines) {
			_normalLines.push_back(QLineF(x, _painterRect.top(), x, _painterRect.bottom()));
		}
		if ((m_grid.getGridFlags() & Grid::ShowWideLines) && (m_grid.getWideGridLineCounter().x() > 1)) {
			// Wide line
			if (lineCounterX >= m_grid.getWideGridLineCounter().x()) {
				_wideLines.push_back(QLineF(x, _painterRect.top(), x, _painterRect.bottom()));
				lineCounterX = 0;
			}
			lineCounterX++;
		}
	}

	// Calculate Y axis lines	
	for (qreal y = startPos.y(); y < _painterRect.bottom(); y += scaledStepSize.y())
	{
		if (m_grid.getGridFlags() & Grid::ShowNormalLines) {
			// Small line only
			_normalLines.push_back(QLineF(_painterRect.left(), y, _painterRect.right(), y));
		}
		if ((m_grid.getGridFlags() & Grid::ShowWideLines) && (m_grid.getWideGridLineCounter().y() > 1)) {
			// Wide line
			if (lineCounterY >= m_grid.getWideGridLineCounter().y()) {
				_wideLines.push_back(QLineF(_painterRect.left(), y, _painterRect.right(), y));
				lineCounterY = 0;
			}
			lineCounterY++;
		}
	}
}

ot::Point2D ot::GraphicsScene::calculateScaledGridStepSize(const QRectF& _rect) const {
	Point2D scaledStepSize = m_grid.getGridStep();
	if (m_grid.getGridFlags() & Grid::AutoScaleGrid) {
		while ((_rect.width() / scaledStepSize.x()) > 200 || (_rect.height() / scaledStepSize.y()) > 200) {
			scaledStepSize.setX(scaledStepSize.x() * 10);
			scaledStepSize.setY(scaledStepSize.y() * 10);
		}
	}
	return scaledStepSize;
}