// @otlicense
// File: GraphicsConnectionItem.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin Core header
#include "OTCore/Math.h"
#include "OTCore/LogDispatcher.h"

// OpenTwin Gui header
#include "OTGui/GraphicsConnectionCalculationData.h"

// OpenTwin Widgets header
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/GraphicsDisconnectItem.h"
#include "OTWidgets/GraphicsConnectionConnectorItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtWidgets/qgraphicssceneevent.h>

ot::GraphicsConnectionItem::GraphicsConnectionItem() :
	m_dest(nullptr), m_origin(nullptr), m_originConnector(nullptr), m_destConnector(nullptr),
	m_disconnectOffset(-(GraphicsDisconnectItem::getSize() / 2.), -(GraphicsDisconnectItem::getSize() / 2.))
{
	m_originDisconnect = new GraphicsDisconnectItem(this);
	m_originDisconnect->setVisible(false);
	m_destDisconnect = new GraphicsDisconnectItem(this);
	m_destDisconnect->setVisible(false);

	this->setFlag(QGraphicsItem::ItemIsSelectable, true);
	this->setAcceptHoverEvents(true);
	this->updateConnectors();
	this->updateOriginConnectionInformation();
	this->updateDestConnectionInformation();
	this->setZValue(GraphicsZValues::Connection);
}

ot::GraphicsConnectionItem::~GraphicsConnectionItem() {
	this->setGraphicsElementDestroying();

	if (this->getGraphicsScene()) {
		this->getGraphicsScene()->connectionAboutToBeRemoved(this);
	}

	if (m_origin) {
		m_origin->forgetConnection(this);
		m_origin = nullptr;
	}
	if (m_dest) {
		m_dest->forgetConnection(this);
		m_dest = nullptr;
	}
	if (m_originConnector) {
		m_originConnector->setGraphicsElementDestroying();
		m_originConnector->forgetConnection(this);
		delete m_originConnector;
		m_originConnector = nullptr;
	}
	if (m_destConnector) {
		m_destConnector->setGraphicsElementDestroying();
		m_destConnector->forgetConnection(this);
		delete m_destConnector;
		m_destConnector = nullptr;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// QGraphicsItem

QRectF ot::GraphicsConnectionItem::boundingRect() const {
	QPainterPath path;
	this->calculatePainterPath(path);
	double margs = m_config.getLineWidth() / 2.;
	return path.boundingRect().marginsAdded(
		QMarginsF(margs, margs, margs, margs));
}

void ot::GraphicsConnectionItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {

	QPen linePen = QtFactory::toQPen(m_config.getLineStyle());

	if (m_config.getHandlesState()) {
		if (this->getGraphicsElementState() & GraphicsElement::HoverState) {
			const Painter2D* newPainter = GraphicsItem::createHoverBorderPainter();
			linePen.setBrush(QtFactory::toQBrush(newPainter));
			delete newPainter;
		}
		else if (this->getGraphicsElementState() & GraphicsElement::SelectedState) {
			const Painter2D* newPainter = GraphicsItem::createSelectionBorderPainter();
			linePen.setBrush(QtFactory::toQBrush(newPainter));
			delete newPainter;
		}
	}

	_painter->setPen(linePen);

	QPainterPath path;
	this->calculatePainterPath(path);
	if (path.isEmpty()) return;
	_painter->drawPath(path);
}

QVariant ot::GraphicsConnectionItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	switch (_change)
	{
	case QGraphicsItem::ItemSelectedHasChanged:
		if (this->isSelected()) {
			this->setGraphicsElementState(GraphicsElement::SelectedState, true);
		}
		else {
			this->setGraphicsElementState(GraphicsElement::SelectedState, false);
		}
		break;
	default:
		break;
	}
	return QGraphicsItem::itemChange(_change, _value);
}

void ot::GraphicsConnectionItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	if (_event->button() == Qt::LeftButton && this->flags() & QGraphicsItem::ItemIsSelectable) {
		GraphicsScene* sc = dynamic_cast<GraphicsScene*>(this->scene());
		if (!sc) {
			OT_LOG_EA("Scene cast failed");
			return;
		}
		bool ignored = sc->getIgnoreEvents();
		bool blocked = sc->signalsBlocked();
		sc->setIgnoreEvents(true);
		sc->blockSignals(true);

		if (_event->modifiers() != Qt::ControlModifier || !sc->getMultiselectionEnabled()) {
			sc->clearSelection();
		}
		this->setSelected(true);

		sc->setIgnoreEvents(ignored);
		sc->blockSignals(blocked);

		sc->handleSelectionChanged();
	}
}

void ot::GraphicsConnectionItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {

}

void ot::GraphicsConnectionItem::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	//this->setGraphicsElementState(GraphicsElement::HoverState, true);
}

void ot::GraphicsConnectionItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	//this->setGraphicsElementState(GraphicsElement::HoverState, false);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Graphics Base

qreal ot::GraphicsConnectionItem::calculateShortestDistanceToPoint(const QPointF& _pt) const {
	qreal distance = -1.;

	switch (m_config.getLineShape())
	{
	case GraphicsConnectionCfg::ConnectionShape::DirectLine:
		distance = this->calculateShortestDistanceToPointDirect(_pt);
		break;
	case GraphicsConnectionCfg::ConnectionShape::SmoothLine:
		distance = this->calculateShortestDistanceToPointSmooth(_pt);
		break;
	case GraphicsConnectionCfg::ConnectionShape::XYLine:
		distance = this->calculateShortestDistanceToPointXY(_pt);
		break;
	case GraphicsConnectionCfg::ConnectionShape::YXLine:
		distance = this->calculateShortestDistanceToPointYX(_pt);
		break;
	case GraphicsConnectionCfg::ConnectionShape::AutoXYLine:
		distance = this->calculateShortestDistanceToPointAutoXY(_pt);
		break;
	default:
		OT_LOG_EA("Unknown line shape");
		break;
	}

	return distance;
}

QGraphicsItem* ot::GraphicsConnectionItem::getQGraphicsItem() {
	return this;
}

const QGraphicsItem* ot::GraphicsConnectionItem::getQGraphicsItem() const {
	return this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Configuration

bool ot::GraphicsConnectionItem::setConfiguration(const ot::GraphicsConnectionCfg& _cfg) {
	m_config = _cfg;
	this->updatePositionsFromItems();
	return true;
}

void ot::GraphicsConnectionItem::setLineShape(GraphicsConnectionCfg::ConnectionShape _shape) {
	this->prepareGeometryChange();
	m_config.setLineShape(_shape);
	this->update();
}

void ot::GraphicsConnectionItem::setLineWidth(double _width) {
	this->prepareGeometryChange();
	m_config.setLineWidth(_width);
	this->update();
}

void ot::GraphicsConnectionItem::setLineStyle(const PenFCfg& _style) {
	this->prepareGeometryChange();
	m_config.setLineStyle(_style);
	this->update();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Item handling

void ot::GraphicsConnectionItem::connectItems(GraphicsItem* _origin, GraphicsItem* _dest) {
	OTAssertNullptr(_origin);
	OTAssertNullptr(_dest);
	OTAssert(m_origin == nullptr, "Origin already set");
	OTAssert(m_dest == nullptr, "Destination already set");

	this->prepareGeometryChange();

	m_origin = _origin;
	m_dest = _dest;
	m_origin->storeConnection(this);
	m_dest->storeConnection(this);

	this->updateConnectors();
	this->updateOriginConnectionInformation();
	this->updateDestConnectionInformation();
	this->updatePositionsFromItems();
	this->update();

}

void ot::GraphicsConnectionItem::setOriginItem(GraphicsItem* _origin) {
	OTAssertNullptr(_origin);
	OTAssert(m_origin != _origin, "Origin already set to the given item");
	OTAssert(m_dest != _origin, "Given item is already set as destination");

	this->prepareGeometryChange();

	if (m_origin) {
		OT_LOG_WA("Origin item already set. Replacing...");
		m_origin->forgetConnection(this);
	}

	m_origin = _origin;
	m_origin->storeConnection(this);

	this->updateConnectors();
	this->updateOriginConnectionInformation();
	this->updatePositionsFromItems();
	this->update();
}

void ot::GraphicsConnectionItem::setOriginPos(const Point2DD& _pos) {
	if (m_origin) {
		OT_LOG_WA("Origin item is set. Ignoring position change.");
		return;
	}
	else {
		OTAssertNullptr(m_originConnector);
		this->prepareGeometryChange();
		m_config.setOriginPos(_pos);
		m_originConnector->setPos(QtFactory::toQPoint(_pos) - QPointF(m_originConnector->boundingRect().width() / 2., m_originConnector->boundingRect().height() / 2.));
		this->update();
	}
}

void ot::GraphicsConnectionItem::setDestItem(GraphicsItem* _dest) {
	OTAssertNullptr(_dest);
	OTAssert(m_dest != _dest, "Destination already set to the given item");
	OTAssert(m_origin != _dest, "Given item is already set as origin");
	
	this->prepareGeometryChange();
	
	if (m_dest) {
		OT_LOG_WA("Destination item already set. Replacing...");
		m_dest->forgetConnection(this);
	}

	m_dest = _dest;
	m_dest->storeConnection(this);

	this->updateConnectors();
	this->updateDestConnectionInformation();
	this->updatePositionsFromItems();
	this->update();
}

void ot::GraphicsConnectionItem::setDestPos(const Point2DD& _pos) {
	if (m_dest) {
		OT_LOG_WA("Destination item is set. Ignoring position change.");
		return;
	}
	else {
		OTAssertNullptr(m_destConnector);
		this->prepareGeometryChange();
		m_config.setDestPos(_pos);
		m_destConnector->setPos(QtFactory::toQPoint(_pos) - QPointF(m_destConnector->boundingRect().width() / 2., m_destConnector->boundingRect().height() / 2.));
		this->update();
	}
}

void ot::GraphicsConnectionItem::disconnectItems(bool _updateConfig) {
	this->prepareGeometryChange();

	if (m_origin) {
		m_origin->forgetConnection(this);
		m_origin = nullptr;
	}
	if (m_dest) {
		m_dest->forgetConnection(this);
		m_dest = nullptr;
	}
	
	this->updateConnectors();
	
	if (_updateConfig) {
		this->updateOriginConnectionInformation();
		this->updateDestConnectionInformation();
	}

	this->update();
}

void ot::GraphicsConnectionItem::disconnectItem(const GraphicsItem* _item, bool _updateConfig) {
	this->prepareGeometryChange();

	bool wasOrigin = false;
	if (m_origin == _item) {
		m_origin->forgetConnection(this);
		m_origin = nullptr;
		wasOrigin = true;
	}

	bool wasDest = false;
	if (m_dest == _item) {
		m_dest->forgetConnection(this);
		m_dest = nullptr;
		wasDest = true;
	}
	
	this->updateConnectors();

	if (_updateConfig) {
		if (wasOrigin) {
			this->updateOriginConnectionInformation();
		}
		if (wasDest) {
			this->updateDestConnectionInformation();
		}
	}

	this->update();
}

void ot::GraphicsConnectionItem::disconnectRequested(GraphicsDisconnectItem* _disconnector) {
	GraphicsScene* sc = this->getGraphicsScene();
	OTAssertNullptr(sc);

	GraphicsView* view = sc->getGraphicsView();
	OTAssertNullptr(view);

	if (_disconnector == m_originDisconnect) {
		this->disconnectItem(m_origin, true);
		view->notifyConnectionChanged(this);
	}
	else if (_disconnector == m_destDisconnect) {
		this->disconnectItem(m_dest, true);
		view->notifyConnectionChanged(this);
	}
	else {
		OT_LOG_EA("Unknown disconnector");
	}
}

void ot::GraphicsConnectionItem::updatePositionsFromItems() {
	m_config.setOriginPos(QtFactory::toPoint2D(this->getOriginPos()));
	m_config.setDestPos(QtFactory::toPoint2D(this->getDestPos()));
}

void ot::GraphicsConnectionItem::updateConnectionView() {
	m_originDisconnect->setPos(this->getOriginPos() + m_disconnectOffset);
	m_destDisconnect->setPos(this->getDestPos() + m_disconnectOffset);

	this->prepareGeometryChange();
	this->update();
}

QPointF ot::GraphicsConnectionItem::getOriginPos() const {
	QGraphicsItem* itm = nullptr;
	if (m_origin) {
		// Origin set
		itm = m_origin->getQGraphicsItem();
	}
	else {
		// Origin not set
		OTAssertNullptr(m_originConnector);
		itm = m_originConnector->getQGraphicsItem();
	}

	OTAssertNullptr(itm);
	return itm->mapToScene(itm->boundingRect().center());
}

QPointF ot::GraphicsConnectionItem::getDestPos() const {
	QGraphicsItem* itm = nullptr;
	if (m_dest) {
		// Destination set
		itm = m_dest->getQGraphicsItem();
	}
	else {
		// Destination not set
		OTAssertNullptr(m_destConnector);
		itm = m_destConnector->getQGraphicsItem();
	}

	OTAssertNullptr(itm);
	return itm->mapToScene(itm->boundingRect().center());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::GraphicsConnectionItem::graphicsSceneSet(GraphicsScene* _scene) {
	if (m_originConnector) {
		_scene->addItem(m_originConnector);
		m_originConnector->setGraphicsScene(_scene);
		m_originConnector->finalizeGraphicsItem();
	}
	if (m_destConnector) {
		_scene->addItem(m_destConnector);
		m_destConnector->setGraphicsScene(_scene);
		m_destConnector->finalizeGraphicsItem();
	}

	_scene->addItem(m_originDisconnect);
	_scene->addItem(m_destDisconnect);

	this->update();
}

void ot::GraphicsConnectionItem::graphicsElementStateChanged(const GraphicsElementStateFlags& _flags) {
	GraphicsElement::graphicsElementStateChanged(_flags);

	updateDisconnectVisibility();

	this->update();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

void ot::GraphicsConnectionItem::updateOriginConnectionInformation() {
	if (m_origin) {
		m_config.setOriginConnectable(m_origin->getGraphicsItemName());
		m_config.setOriginUid(m_origin->getRootItem()->getGraphicsItemUid());
	}
	else {
		m_config.setOriginConnectable("");
		m_config.setOriginUid(0);
	}
}

void ot::GraphicsConnectionItem::updateDestConnectionInformation() {
	if (m_dest) {
		m_config.setDestConnectable(m_dest->getGraphicsItemName());
		m_config.setDestUid(m_dest->getRootItem()->getGraphicsItemUid());
	}
	else {
		m_config.setDestConnectable("");
		m_config.setDestUid(0);
	}
}

ot::ConnectionDirection ot::GraphicsConnectionItem::calculateOriginDirection() const {
	if (m_origin) {
		return m_origin->getConnectionDirection();
	}
	else {
		OTAssertNullptr(m_originConnector);
		return m_originConnector->getConnectionDirection();
	}
}

ot::ConnectionDirection ot::GraphicsConnectionItem::calculateDestDirection() const {
	if (m_dest) {
		return m_dest->getConnectionDirection();
	}
	else {
		OTAssertNullptr(m_destConnector);
		return m_destConnector->getConnectionDirection();
	}
}

void ot::GraphicsConnectionItem::updateConnectors() {
	GraphicsScene* sc = this->getGraphicsScene();

	if (m_origin) {
		// Origin set
		if (m_originConnector) {
			m_originConnector->setGraphicsElementDestroying();
			m_originConnector->forgetConnection(this);

			if (sc) {
				sc->itemAboutToBeRemoved(m_originConnector);
				sc->removeItem(m_originConnector);
				m_originConnector->setGraphicsScene(nullptr);
			}

			delete m_originConnector;
			m_originConnector = nullptr;
		}
	}
	else {
		// Origin not set
		if (!m_originConnector) {
			m_originConnector = new GraphicsConnectionConnectorItem;
			if (sc) {
				sc->addItem(m_originConnector);
				m_originConnector->setGraphicsScene(sc);
				m_originConnector->finalizeGraphicsItem();
			}

			m_originConnector->setPos(QtFactory::toQPoint(m_config.getOriginPos()) - QPointF(m_originConnector->getRadiusX(), m_originConnector->getRadiusY()));
			m_originConnector->storeConnection(this);
		}

	}

	if (m_dest) {
		// Destination set
		if (m_destConnector) {
			m_destConnector->setGraphicsElementDestroying();
			m_destConnector->forgetConnection(this);

			if (sc) {
				sc->itemAboutToBeRemoved(m_destConnector);
				sc->removeItem(m_destConnector);
				m_destConnector->setGraphicsScene(nullptr);
			}
			
			delete m_destConnector;
			m_destConnector = nullptr;
		}
	}
	else {
		// Destination not set
		if (!m_destConnector) {
			m_destConnector = new GraphicsConnectionConnectorItem;
			if (sc) {
				sc->addItem(m_destConnector);
				m_destConnector->setGraphicsScene(sc);
				m_destConnector->finalizeGraphicsItem();
			}

			m_destConnector->setPos(QtFactory::toQPoint(m_config.getDestPos()) - QPointF(m_destConnector->getRadiusX(), m_destConnector->getRadiusY()));
			m_destConnector->storeConnection(this);
		}
	}

	this->updateDisconnectVisibility();
}

void ot::GraphicsConnectionItem::updateDisconnectVisibility() {
	if (this->getGraphicsElementState() & GraphicsElement::SelectedState) {
		if (m_origin) {
			//m_originDisconnect->setVisible(true);
		}
		else {
			m_originDisconnect->setVisible(false);
		}
		if (m_dest) {
			//m_destDisconnect->setVisible(true);
		}
		else {
			m_destDisconnect->setVisible(false);
		}
	}
	else {
		m_originDisconnect->setVisible(false);
		m_destDisconnect->setVisible(false);
	}
}

void ot::GraphicsConnectionItem::calculatePainterPath(QPainterPath& _path) const {
	switch (m_config.getLineShape())
	{
	case ot::GraphicsConnectionCfg::ConnectionShape::DirectLine:
		this->calculateDirectLinePath(_path);
		break;
	case ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine:
		this->calculateSmoothLinePath(_path);
		break;
	case ot::GraphicsConnectionCfg::ConnectionShape::XYLine:
		this->calculateXYLinePath(_path);
		break;
	case ot::GraphicsConnectionCfg::ConnectionShape::YXLine:
		this->calculateYXLinePath(_path);
		break;
	case ot::GraphicsConnectionCfg::ConnectionShape::AutoXYLine:
		this->calculateAutoXYLinePath(_path);
		break;
	default:
		OT_LOG_E("Unknown connection shape (" + std::to_string((int)m_config.getLineShape()) + ")");
		break;
	}
}

void ot::GraphicsConnectionItem::calculateDirectLinePath(QPainterPath& _path) const {
	_path.moveTo(this->getOriginPos());
	_path.lineTo(this->getDestPos());
}

void ot::GraphicsConnectionItem::calculateSmoothLinePath(QPainterPath& _path) const {
	// Get the center point of the connectable items
	const QPointF originPoint = this->getOriginPos();
	const QPointF destinationPoint = this->getDestPos();

	// Calculate distance between the items
	const double halfdistX = (std::max(originPoint.x(), destinationPoint.x()) - std::min(originPoint.x(), destinationPoint.x())) / 2.;
	const double halfdistY = (std::max(originPoint.y(), destinationPoint.y()) - std::min(originPoint.y(), destinationPoint.y())) / 2.;

	// Calculate control points
	QPointF controlPoint1;
	QPointF controlPoint2;
	this->calculateSmoothLineStep(originPoint, destinationPoint, halfdistX, halfdistY, controlPoint1, this->calculateOriginDirection());
	this->calculateSmoothLineStep(destinationPoint, originPoint, halfdistX, halfdistY, controlPoint2, this->calculateDestDirection());

	_path.moveTo(originPoint);
	_path.cubicTo(controlPoint1, controlPoint2, destinationPoint);
}

void ot::GraphicsConnectionItem::calculateSmoothLineStep(const QPointF& _origin, const QPointF& _destination, double _halfdistX, double _halfdistY, QPointF& _control, ot::ConnectionDirection _direction) const {
	switch (_direction) {
	case ConnectionDirection::Any:
	{
		double ptx, pty;
		if (_origin.x() > _destination.x()) ptx = _origin.x() - _halfdistX;
		else ptx = _origin.x() + _halfdistX;
		if (_origin.y() > _destination.y()) pty = _origin.y() - _halfdistY;
		else pty = _origin.y() + _halfdistY;
		_control = QPointF(_origin.x() + ptx, _origin.y() + pty);
	}
		break;
	case ConnectionDirection::Left:
		_control = QPointF(_origin.x() - _halfdistX, _origin.y());
		break;
	case ConnectionDirection::Up:
		_control = QPointF(_origin.x(), _origin.y() - _halfdistY);
		break;
	case ConnectionDirection::Right:
		_control = QPointF(_origin.x() + _halfdistX, _origin.y());
		break;
	case ConnectionDirection::Down:
		_control = QPointF(_origin.x(), _origin.y() + _halfdistY);
		break;
	default:
		OT_LOG_EA("Unknown connection direction");
		break;
	}
}

void ot::GraphicsConnectionItem::calculateXYLinePath(QPainterPath& _path) const {
	const QPointF originPoint = this->getOriginPos();
	const QPointF destinationPoint = this->getDestPos();
	const QPointF controlPoint(destinationPoint.x(), originPoint.y());

	_path.moveTo(originPoint);

	if (destinationPoint != controlPoint) {
		_path.lineTo(controlPoint);
	}
	_path.lineTo(destinationPoint);
}

void ot::GraphicsConnectionItem::calculateYXLinePath(QPainterPath& _path) const {
	const QPointF originPoint = this->getOriginPos();
	const QPointF destinationPoint = this->getDestPos();
	const QPointF controlPoint(originPoint.x(), destinationPoint.y());

	_path.moveTo(originPoint);

	if (destinationPoint != controlPoint) {
		_path.lineTo(controlPoint);
	}
	_path.lineTo(destinationPoint);
}

void ot::GraphicsConnectionItem::calculateAutoXYLinePath(QPainterPath& _path) const {
	switch (this->calculateAutoXYShape())
	{
	case ot::GraphicsConnectionCfg::ConnectionShape::XYLine:
		this->calculateXYLinePath(_path);
		break;

	case ot::GraphicsConnectionCfg::ConnectionShape::YXLine:
		this->calculateYXLinePath(_path);
		break;

	default:
		OT_LOG_WA("Invalid calculated shape");
		this->calculateDirectLinePath(_path);
		break;
	}
}

qreal ot::GraphicsConnectionItem::calculateShortestDistanceToPointDirect(const QPointF& _pt) const {
	const QPointF originPoint = this->getOriginPos();
	const QPointF destinationPoint = this->getDestPos();

	return Math::calculateShortestDistanceFromPointToLine(_pt.x(), _pt.y(), originPoint.x(), originPoint.y(), destinationPoint.x(), destinationPoint.y());
}

qreal ot::GraphicsConnectionItem::calculateShortestDistanceToPointSmooth(const QPointF& _pt) const {
	const QPointF originPoint = this->getOriginPos();
	const QPointF destinationPoint = this->getDestPos();

	// Calculate distance between the items
	double halfdistX = (std::max(originPoint.x(), destinationPoint.x()) - std::min(originPoint.x(), destinationPoint.x())) / 2.;
	double halfdistY = (std::max(originPoint.y(), destinationPoint.y()) - std::min(originPoint.y(), destinationPoint.y())) / 2.;

	// Calculate control points
	QPointF controlPoint1;
	QPointF controlPoint2;
	this->calculateSmoothLineStep(originPoint, destinationPoint, halfdistX, halfdistY, controlPoint1, this->calculateOriginDirection());
	this->calculateSmoothLineStep(destinationPoint, originPoint, halfdistX, halfdistY, controlPoint2, this->calculateDestDirection());

	return Math::calculateShortestDistanceFromPointToBezierCurve(_pt.x(), _pt.y(), originPoint.x(), originPoint.y(), controlPoint1.x(), controlPoint1.y(), controlPoint2.x(), controlPoint2.y(), destinationPoint.x(), destinationPoint.y());
}

qreal ot::GraphicsConnectionItem::calculateShortestDistanceToPointXY(const QPointF& _pt) const {
	const QPointF originPoint = this->getOriginPos();
	const QPointF destinationPoint = this->getDestPos();
	const QPointF controlPoint(destinationPoint.x(), originPoint.y());

	qreal d1 = Math::calculateShortestDistanceFromPointToLine(_pt.x(), _pt.y(), originPoint.x(), originPoint.y(), controlPoint.x(), controlPoint.y());
	qreal d2 = Math::calculateShortestDistanceFromPointToLine(_pt.x(), _pt.y(), controlPoint.x(), controlPoint.y(), destinationPoint.x(), destinationPoint.y());

	return std::min(d1, d2);
}

qreal ot::GraphicsConnectionItem::calculateShortestDistanceToPointYX(const QPointF& _pt) const {
	const QPointF originPoint = this->getOriginPos();
	const QPointF destinationPoint = this->getDestPos();
	const QPointF controlPoint(originPoint.x(), destinationPoint.y());

	qreal d1 = Math::calculateShortestDistanceFromPointToLine(_pt.x(), _pt.y(), originPoint.x(), originPoint.y(), controlPoint.x(), controlPoint.y());
	qreal d2 = Math::calculateShortestDistanceFromPointToLine(_pt.x(), _pt.y(), controlPoint.x(), controlPoint.y(), destinationPoint.x(), destinationPoint.y());

	return std::min(d1, d2);
}

qreal ot::GraphicsConnectionItem::calculateShortestDistanceToPointAutoXY(const QPointF& _pt) const
{
	switch (this->calculateAutoXYShape())
	{
	case ot::GraphicsConnectionCfg::ConnectionShape::XYLine:
		return this->calculateShortestDistanceToPointXY(_pt);
	case ot::GraphicsConnectionCfg::ConnectionShape::YXLine:
		return this->calculateShortestDistanceToPointYX(_pt);
	default:
		OT_LOG_WA("Invalid calculated shape");
		return this->calculateShortestDistanceToPointDirect(_pt);
	}
}

ot::GraphicsConnectionCfg::ConnectionShape ot::GraphicsConnectionItem::calculateAutoXYShape() const
{
	Alignment connectionAlignment = this->calculateConnectionDirectionAlignment();
	
	if (
		connectionAlignment == Alignment::Left ||
		connectionAlignment == Alignment::Top ||
		connectionAlignment == Alignment::Right ||
		connectionAlignment == Alignment::Bottom ||
		connectionAlignment == Alignment::Center
		) {
		return GraphicsConnectionCfg::ConnectionShape::XYLine;
	}

	GraphicsConnectionCalculationData info(this->calculateOriginDirection(), this->calculateDestDirection(), connectionAlignment);
	std::list<GraphicsConnectionCalculationData> yxLines = GraphicsConnectionCalculationData::getAllYXLines();
	
	if (std::find(yxLines.begin(), yxLines.end(), info) != yxLines.end()) {
		return GraphicsConnectionCfg::ConnectionShape::YXLine;
	}
	else {
		return GraphicsConnectionCfg::ConnectionShape::XYLine;
	}
}

ot::Alignment ot::GraphicsConnectionItem::calculateConnectionDirectionAlignment() const {
	const QPointF originPoint = this->getOriginPos();
	const QPointF destinationPoint = this->getDestPos();
	const QPointF delta = destinationPoint - originPoint;

	if (delta.x() > 0.) {
		if (delta.y() > 0.) return Alignment::BottomRight;
		else if (delta.y() < 0.) return Alignment::TopRight;
		else return Alignment::Right;
	}
	else if (delta.x() < 0.) {
		if (delta.y() > 0.) return Alignment::BottomLeft;
		else if (delta.y() < 0.) return Alignment::TopLeft;
		else return Alignment::Left;
	}
	else {
		if (delta.y() > 0.) return Alignment::Bottom;
		else if (delta.y() < 0.) return Alignment::Top;
		else return Alignment::Center;
	}
}
