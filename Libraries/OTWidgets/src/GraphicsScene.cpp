//! @file GraphicsScene.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
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

ot::GraphicsScene::GraphicsScene(GraphicsView* _view) :
	m_view(_view), m_connectionOrigin(nullptr), m_connectionPreview(nullptr),
	m_connectionPreviewShape(ot::GraphicsConnectionCfg::ConnectionShape::DirectLine), m_ignoreEvents(false), m_mouseIsPressed(false),
	m_maxTriggerDistance(0.), m_multiselectionEnabled(true)
{
	OTAssertNullptr(m_view);

	this->setItemIndexMethod(QGraphicsScene::NoIndex);

	this->connect(this, &GraphicsScene::selectionChanged, this, &GraphicsScene::slotSelectionChanged);
}

ot::GraphicsScene::GraphicsScene(const QRectF& _sceneRect, GraphicsView* _view)
	: QGraphicsScene(_sceneRect), m_view(_view), m_connectionOrigin(nullptr), m_connectionPreview(nullptr),
	m_connectionPreviewShape(ot::GraphicsConnectionCfg::ConnectionShape::DirectLine), m_ignoreEvents(false), m_mouseIsPressed(false),
	m_maxTriggerDistance(0.), m_multiselectionEnabled(true)
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
	UID originUid = m_connectionOrigin->getRootItem()->getGraphicsItemUid();
	std::string originConnectable = m_connectionOrigin->getGraphicsItemName();
	UID targetUid = _targetedConnection->getConfiguration().getUid();
	this->stopConnection();
	m_view->requestConnectionToConnection(originUid, originConnectable, targetUid, _pos);
}

void ot::GraphicsScene::startConnection(ot::GraphicsItem* _item) {
	OTAssertNullptr(m_view);
	if (m_view->getGraphicsViewFlags() & GraphicsView::IgnoreConnectionByUser) return;

	// Ignore new connection on read only mode
	if (m_view->isReadOnly()) {
		this->stopConnection(); // Stop connection if read only was set during drag
		return;
	}

	if (m_connectionOrigin == nullptr) {
		// Start new connection
		m_connectionOrigin = _item;

		m_connectionPreview = new GraphicsConnectionPreviewItem;
		QPen p;
		p.setColor(QColor(64, 64, 255));
		p.setWidth(1);
		m_connectionPreview->setConnectionShape(m_connectionPreviewShape);
		m_connectionPreview->setOriginPos(m_connectionOrigin->getQGraphicsItem()->mapToScene(m_connectionOrigin->getQGraphicsItem()->boundingRect().center()));
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
		UID originUid = m_connectionOrigin->getRootItem()->getGraphicsItemUid();
		std::string originConnectable = m_connectionOrigin->getGraphicsItemName();
		UID destUid = _item->getRootItem()->getGraphicsItemUid();
		std::string destConnectable = _item->getGraphicsItemName();
		this->stopConnection();
		m_view->requestConnection(originUid, originConnectable, destUid, destConnectable);
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

QPointF ot::GraphicsScene::snapToGrid(const QPointF& _pt) const {
	return QtFactory::toQPoint(m_grid.snapToGrid(QtFactory::toPoint2D(_pt)));
}

QPointF ot::GraphicsScene::snapToGrid(const GraphicsItem* _item) const {
	QPointF newPos = _item->getQGraphicsItem()->pos();
	if (_item->getGraphicsItemFlags() & GraphicsItemCfg::ItemSnapsToGridTopLeft) {
		newPos = this->snapToGrid(newPos);
	}
	else if (_item->getGraphicsItemFlags() & GraphicsItemCfg::ItemSnapsToGridCenter) {
		QRectF rect = _item->getQGraphicsItem()->boundingRect();
		QPointF delta(rect.width() / 2., rect.height() / 2.);
		newPos = this->snapToGrid(newPos + delta) - delta;
	}
	return newPos;
}

void ot::GraphicsScene::deselectAll(void) {
	bool blocked = this->signalsBlocked();
	this->blockSignals(true);
	for (QGraphicsItem* itm : this->selectedItems()) {
		itm->setSelected(false);
	}
	this->blockSignals(blocked);
	Q_EMIT selectionChanged();
}

void ot::GraphicsScene::moveAllSelectedItems(const Point2DD& _delta) {
	if (_delta.x() == 0. && _delta.y() == 0.) return;
	for (QGraphicsItem* item : this->selectedItems()) {
		GraphicsItem* otItem = dynamic_cast<GraphicsItem*>(item);
		if (otItem) {
			if (otItem->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsMoveable) {
				if (otItem->getGraphicsItemFlags() & GraphicsItemCfg::ItemSnapsToGridTopLeft) {
					QPointF newPos = QtFactory::toQPoint(m_grid.snapToGrid(otItem->getGraphicsItemPos() + _delta));;
					otItem->setGraphicsItemPos(newPos);
				}
				else if (otItem->getGraphicsItemFlags() & GraphicsItemCfg::ItemSnapsToGridCenter) {
					QRectF rect = otItem->getQGraphicsItem()->boundingRect();
					rect.translate(_delta.x(), _delta.y());
					QPointF newPos = QtFactory::toQPoint(m_grid.snapToGrid(QtFactory::toPoint2D(rect.center()))) - QPointF(rect.width() / 2., rect.height() / 2.);
					otItem->setGraphicsItemPos(newPos);
				}
				else {
					otItem->setGraphicsItemPos(otItem->getGraphicsItemPos() + _delta);
				}
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
	this->elementAboutToBeRemoved(_item);
	if (_item == m_connectionOrigin) {
		m_connectionOrigin = nullptr;
		this->stopConnection();
	}
}

void ot::GraphicsScene::connectionAboutToBeRemoved(GraphicsConnectionItem* _connection) {
	this->elementAboutToBeRemoved(_connection);
}

void ot::GraphicsScene::elementAboutToBeRemoved(GraphicsElement* _element) {
	std::list<GraphicsElement*> allElements = _element->getAllGraphicsElements();

	for (GraphicsElement* element : allElements) {
		auto it = std::find(m_lastHoverElements.begin(), m_lastHoverElements.end(), element);
		while (it != m_lastHoverElements.end()) {
			m_lastHoverElements.erase(it);
			it = std::find(m_lastHoverElements.begin(), m_lastHoverElements.end(), element);
		}
	}
}

void ot::GraphicsScene::handleSelectionChanged(void) {
	if (m_multiselectionEnabled) this->handleMultiSelectionChanged();
	else this->handleSingleSelectionChanged();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

void ot::GraphicsScene::slotSelectionChanged(void) {
	if (m_ignoreEvents || m_mouseIsPressed) return;
	this->handleSelectionChanged();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Event handling

void ot::GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) {
	// Check for a new connection
	GraphicsElement* targetedElement = this->findClosestConnectableElement(_event->scenePos());

	// Check if a base item was found next to the click position.
	if (targetedElement) {
		GraphicsItem* graphicsItem = dynamic_cast<ot::GraphicsItem*>(targetedElement);
		GraphicsConnectionItem* connectionItem = dynamic_cast<GraphicsConnectionItem*>(targetedElement);

		if (graphicsItem) {
			this->startConnection(graphicsItem);
		}
		else if (connectionItem) {
			this->startConnectionToConnection(connectionItem, QtFactory::toPoint2D(_event->scenePos()));
		}
	}
	else {
		this->stopConnection();

		QList<QGraphicsItem*> itms = this->items(_event->scenePos());
		ot::GraphicsItem* itemUnder = nullptr;
		for (QGraphicsItem* itm : itms) {
			ot::GraphicsItem* actualItem = dynamic_cast<ot::GraphicsItem*>(itm);
			if (actualItem) {
				if (!actualItem->getParentGraphicsItem()) {
					if (itemUnder) return;
					else itemUnder = actualItem;
				}
			}
		}

		if (itemUnder) {
			Q_EMIT graphicsItemDoubleClicked(itemUnder);
		}
	}
}

void ot::GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _event) {
	if (m_connectionPreview) {
		OTAssertNullptr(m_connectionOrigin);
		QGraphicsItem* graphicsItem = m_connectionOrigin->getQGraphicsItem();
		OTAssertNullptr(graphicsItem);
		if (graphicsItem != nullptr) //There was a bug where the m_connectionOrigin existed, but the graphicsItem was deleted
		{
			const auto& graphicsItemCenter = graphicsItem->boundingRect().center();
			graphicsItem->mapToScene(graphicsItemCenter);
		}
		else
		{
			assert(0);
		}

		m_connectionPreview->setDestPos(_event->scenePos());
	}

	std::list<GraphicsElement*> clearHoverElements;
	std::list<GraphicsElement*> newHoverElements;

	// First check for a connectable close by
	GraphicsElement* targetedElement = this->findClosestConnectableElement(_event->scenePos());
	
	if (targetedElement) {
		newHoverElements.push_back(targetedElement);
	}
	else {
		// No connectable, check for root items and connections at pos
		QList<QGraphicsItem*> hoverItems = this->items(_event->scenePos());
		for (QGraphicsItem* itm : hoverItems) {
			GraphicsItem* actualItem = dynamic_cast<GraphicsItem*>(itm);
			GraphicsConnectionItem* actualConnection = dynamic_cast<GraphicsConnectionItem*>(itm);
			if (actualItem) {
				if (!actualItem->getParentGraphicsItem() && (actualItem->getGraphicsItemFlags() & (GraphicsItemCfg::ItemHandlesState | GraphicsItemCfg::ItemForwardsState))) {
					newHoverElements.push_back(actualItem);
				}
			}
			else if (actualConnection) {
				newHoverElements.push_back(actualConnection);
			}
			else if (itm != m_connectionPreview) {
				OT_LOG_WA("Unknown item in scene");
			}
		}
	}

	// Check for state reset
	for (GraphicsElement* element : m_lastHoverElements) {
		if (std::find(newHoverElements.begin(), newHoverElements.end(), element) == newHoverElements.end()) {
			clearHoverElements.push_back(element);
		}
	}
	
	// Apply state
	m_lastHoverElements = newHoverElements;

	for (GraphicsElement* element : clearHoverElements) {
		element->setGraphicsElementState(GraphicsElement::HoverState, false);
	}
	for (GraphicsElement* element : m_lastHoverElements) {
		element->setGraphicsElementState(GraphicsElement::HoverState, true);
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

	pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::Border).toBrush());
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
		pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::WidgetHoverBackground).toBrush());
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

ot::GraphicsElement* ot::GraphicsScene::findClosestConnectableElement(const QPointF& _pos) const {
	QList<QGraphicsItem*> lst = this->findItemsInTriggerDistance(_pos);

	qreal minDistance = std::numeric_limits<double>::max();
	GraphicsElement* targetedElement = nullptr;
	bool targetedIsItem = false;
	for (auto itm : lst) {
		ot::GraphicsElement* actualBase = dynamic_cast<ot::GraphicsElement*>(itm);
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

			qreal dist = actualBase->calculateShortestDistanceToPoint(_pos);

			// Item
			if (actualItm) {
				if ((actualItm->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable) && dist >= 0. && ((dist < minDistance && targetedIsItem) || !targetedIsItem)) {
					minDistance = dist;
					targetedElement = actualBase;
					targetedIsItem = true;
				}
			}
			// Connection
			else if (dist >= 0. && dist < minDistance && !targetedIsItem) {
				minDistance = dist;
				targetedElement = actualBase;
			}
		}
	}

	return targetedElement;
}

QList<QGraphicsItem*> ot::GraphicsScene::findItemsInTriggerDistance(const QPointF& _pos) const {
	QList<QGraphicsItem*> lst;
	if (m_maxTriggerDistance > 0.) {
		QRectF hitRect(
			_pos.x() - m_maxTriggerDistance,
			_pos.y() - m_maxTriggerDistance,
			m_maxTriggerDistance * 2.,
			m_maxTriggerDistance * 2.
		);

		lst = this->items(hitRect);
	}
	else {
		lst = this->items(_pos);
	}
	return lst;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::GraphicsScene::handleMultiSelectionChanged(void) {
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

void ot::GraphicsScene::handleSingleSelectionChanged(void) {
	QList<QGraphicsItem*> deselectList = m_lastSelection;
	QList<QGraphicsItem*> selectList = this->selectedItems();
	
	// Check if the selection did not change
	if (deselectList.size() == selectList.size()) {
		bool hasMissing = false;
		for (QGraphicsItem* itm : deselectList) {
			auto it = std::find(selectList.begin(), selectList.end(), itm);
			if (it == selectList.end()) {
				hasMissing = true;
				break;
			}
		}
		if (!hasMissing) return;
	}

	// Remove already selected items from new selection
	for (QGraphicsItem* itm : deselectList) {
		auto it = std::find(selectList.begin(), selectList.end(), itm);
		if (it != selectList.end()) {
			selectList.erase(it);
		}
	}

	bool blocked = this->signalsBlocked();
	this->blockSignals(true);

	// Check if more than one item is selected, if so deselect all
	if (selectList.size() > 1) {
		for (QGraphicsItem* itm : selectList) {
			auto it = std::find(deselectList.begin(), deselectList.end(), itm);
			if (it == deselectList.end()) deselectList.push_back(itm);
		}
		selectList.clear();
	}
	else {
		// Remove items that are now selected from the deselect list
		for (QGraphicsItem* itm : selectList) {
			auto it = std::find(deselectList.begin(), deselectList.end(), itm);
			if (it != deselectList.end()) deselectList.erase(it);
		}
	}

	// Deselect
	for (QGraphicsItem* itm : deselectList) {
		itm->setSelected(false);
	}

	m_lastSelection = selectList;

	this->blockSignals(blocked);

	Q_EMIT selectionChangeFinished();
}
