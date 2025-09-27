//! @file GraphicsConnectionItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Math.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsConnectionCalculationData.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsScene.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtWidgets/qgraphicssceneevent.h>

ot::GraphicsConnectionItem::GraphicsConnectionItem() :
	m_dest(nullptr), m_origin(nullptr)
{
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);
	this->setAcceptHoverEvents(true);
}

ot::GraphicsConnectionItem::~GraphicsConnectionItem() {
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
}

// ###########################################################################################################################################################################################################################################################################################################################

// QGraphicsItem

QRectF ot::GraphicsConnectionItem::boundingRect(void) const {
	if (m_dest == nullptr || m_origin == nullptr) {
		return m_lastRect;
	}

	QPainterPath path;
	this->calculatePainterPath(path);
	double margs = m_config.getLineWidth() / 2.;
	return path.boundingRect().marginsAdded(
		QMarginsF(margs, margs, margs, margs));
}

void ot::GraphicsConnectionItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {

	QPen linePen = QtFactory::toQPen(m_config.getLineStyle());

	if (m_config.getHandleState()) {
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

QGraphicsItem* ot::GraphicsConnectionItem::getQGraphicsItem(void) {
	return this;
}

const QGraphicsItem* ot::GraphicsConnectionItem::getQGraphicsItem(void) const {
	return this;
}

// ###########################################################################################################################################################################################################################################################################################################################

bool ot::GraphicsConnectionItem::setConfiguration(const ot::GraphicsConnectionCfg& _cfg) {
	m_config = _cfg;

	return true;
}

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

	this->updateConnectionInformation();
	this->updateConnectionView();
}

void ot::GraphicsConnectionItem::disconnectItems(void) {
	if (m_origin) {
		m_origin->forgetConnection(this);
		m_origin = nullptr;
	}
	if (m_dest) {
		m_dest->forgetConnection(this);
		m_dest = nullptr;
	}
}

void ot::GraphicsConnectionItem::forgetItem(const GraphicsItem* _item) {
	if (m_origin == _item) {
		m_origin = nullptr;
	}
	if (m_dest == _item) {
		m_dest = nullptr;
	}
}

void ot::GraphicsConnectionItem::setHandleState(bool _handlesState) {
	m_config.setHandlesState(_handlesState);
}

bool ot::GraphicsConnectionItem::getHandleState(void) const {
	return m_config.getHandleState();
}

void ot::GraphicsConnectionItem::updateConnectionView(void) {
	if (m_origin && m_dest) {
		this->prepareGeometryChange();
		this->update();
	}
}

void ot::GraphicsConnectionItem::updateConnectionInformation(void) {
	if (m_origin) {
		m_config.setOriginConnectable(m_origin->getGraphicsItemName());
		m_config.setOriginUid(m_origin->getRootItem()->getGraphicsItemUid());
	}
	else {
		m_config.setOriginConnectable("");
		m_config.setOriginUid(0);
	}

	if (m_dest) {
		m_config.setDestConnectable(m_dest->getGraphicsItemName());
		m_config.setDestUid(m_dest->getRootItem()->getGraphicsItemUid());
	}
	else {
		m_config.setDestConnectable("");
		m_config.setDestUid(0);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::GraphicsConnectionItem::graphicsElementStateChanged(const GraphicsElementStateFlags& _flags) {
	GraphicsElement::graphicsElementStateChanged(_flags);
	this->update();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

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
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Origin and/or destination not set");
		return;
	}

	//QPointF originPoint = this->originItem()->getQGraphicsItem()->mapToScene(this->originItem()->getQGraphicsItem()->pos() + this->originItem()->getQGraphicsItem()->boundingRect().center());
	//QPointF destinationPoint = this->destItem()->getQGraphicsItem()->mapToScene(this->destItem()->getQGraphicsItem()->pos() + this->destItem()->getQGraphicsItem()->boundingRect().center());

	QPointF originPoint = this->originItem()->getQGraphicsItem()->mapToScene(this->originItem()->getQGraphicsItem()->boundingRect().center());
	QPointF destinationPoint = this->destItem()->getQGraphicsItem()->mapToScene(this->destItem()->getQGraphicsItem()->boundingRect().center());

	_path.moveTo(originPoint);
	_path.lineTo(destinationPoint);

	//_path.moveTo(this->originItem()->getQGraphicsItem()->scenePos() + this->originItem()->getQGraphicsItem()->boundingRect().center());
	//_path.lineTo(this->destItem()->getQGraphicsItem()->scenePos() + this->destItem()->getQGraphicsItem()->boundingRect().center());
}

void ot::GraphicsConnectionItem::calculateSmoothLinePath(QPainterPath& _path) const {
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Origin and/or destination not set");
		return;
	}

	// Get the center point of the connectable items
	QPointF originPoint = this->originItem()->getQGraphicsItem()->mapToScene(this->originItem()->getQGraphicsItem()->boundingRect().center());
	QPointF destinationPoint = this->destItem()->getQGraphicsItem()->mapToScene(this->destItem()->getQGraphicsItem()->boundingRect().center());

	// Calculate distance between the items
	double halfdistX = (std::max(originPoint.x(), destinationPoint.x()) - std::min(originPoint.x(), destinationPoint.x())) / 2.;
	double halfdistY = (std::max(originPoint.y(), destinationPoint.y()) - std::min(originPoint.y(), destinationPoint.y())) / 2.;

	// Calculate control points
	QPointF controlPoint1;
	QPointF controlPoint2;
	this->calculateSmoothLineStep(originPoint, destinationPoint, halfdistX, halfdistY, controlPoint1, this->originItem()->getConnectionDirection());
	this->calculateSmoothLineStep(destinationPoint, originPoint, halfdistX, halfdistY, controlPoint2, this->destItem()->getConnectionDirection());

	_path.moveTo(originPoint);
	_path.cubicTo(controlPoint1, controlPoint2, destinationPoint);
}

void ot::GraphicsConnectionItem::calculateSmoothLineStep(const QPointF& _origin, const QPointF& _destination, double _halfdistX, double _halfdistY, QPointF& _control, ot::ConnectionDirection _direction) const {
	switch (_direction) {
	case ot::ConnectAny:
	{
		double ptx, pty;
		if (_origin.x() > _destination.x()) ptx = _origin.x() - _halfdistX;
		else ptx = _origin.x() + _halfdistX;
		if (_origin.y() > _destination.y()) pty = _origin.y() - _halfdistY;
		else pty = _origin.y() + _halfdistY;
		_control = QPointF(_origin.x() + ptx, _origin.y() + pty);
	}
		break;
	case ot::ConnectLeft:
		_control = QPointF(_origin.x() - _halfdistX, _origin.y());
		break;
	case ot::ConnectUp:
		_control = QPointF(_origin.x(), _origin.y() - _halfdistY);
		break;
	case ot::ConnectRight:
		_control = QPointF(_origin.x() + _halfdistX, _origin.y());
		break;
	case ot::ConnectDown:
		_control = QPointF(_origin.x(), _origin.y() + _halfdistY);
		break;
	default:
		OT_LOG_EA("Unknown connection direction");
		break;
	}
}

void ot::GraphicsConnectionItem::calculateXYLinePath(QPainterPath& _path) const {
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Origin and/or destination not set");
		return;
	}

	QPointF originPoint = this->originItem()->getQGraphicsItem()->mapToScene(this->originItem()->getQGraphicsItem()->boundingRect().center());
	QPointF destinationPoint = this->destItem()->getQGraphicsItem()->mapToScene(this->destItem()->getQGraphicsItem()->boundingRect().center());
	QPointF controlPoint(destinationPoint.x(), originPoint.y());

	_path.moveTo(originPoint);

	if (destinationPoint != controlPoint) {
		_path.lineTo(controlPoint);
	}
	_path.lineTo(destinationPoint);
}

void ot::GraphicsConnectionItem::calculateYXLinePath(QPainterPath& _path) const {
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Origin and/or destination not set");
		return;
	}

	QPointF originPoint = this->originItem()->getQGraphicsItem()->mapToScene(this->originItem()->getQGraphicsItem()->boundingRect().center());
	QPointF destinationPoint = this->destItem()->getQGraphicsItem()->mapToScene(this->destItem()->getQGraphicsItem()->boundingRect().center());
	QPointF controlPoint(originPoint.x(), destinationPoint.y());

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
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Origin and/or destination not set");
		return -1.;
	}

	QPointF originPoint = this->originItem()->getQGraphicsItem()->mapToScene(this->originItem()->getQGraphicsItem()->boundingRect().center());
	QPointF destinationPoint = this->destItem()->getQGraphicsItem()->mapToScene(this->destItem()->getQGraphicsItem()->boundingRect().center());

	return Math::calculateShortestDistanceFromPointToLine(_pt.x(), _pt.y(), originPoint.x(), originPoint.y(), destinationPoint.x(), destinationPoint.y());
}

qreal ot::GraphicsConnectionItem::calculateShortestDistanceToPointSmooth(const QPointF& _pt) const {
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Origin and/or destination not set");
		return -1.;
	}

	// Get the center point of the connectable items
	QPointF originPoint = this->originItem()->getQGraphicsItem()->mapToScene(this->originItem()->getQGraphicsItem()->boundingRect().center());
	QPointF destinationPoint = this->destItem()->getQGraphicsItem()->mapToScene(this->destItem()->getQGraphicsItem()->boundingRect().center());

	// Calculate distance between the items
	double halfdistX = (std::max(originPoint.x(), destinationPoint.x()) - std::min(originPoint.x(), destinationPoint.x())) / 2.;
	double halfdistY = (std::max(originPoint.y(), destinationPoint.y()) - std::min(originPoint.y(), destinationPoint.y())) / 2.;

	// Calculate control points
	QPointF controlPoint1;
	QPointF controlPoint2;
	this->calculateSmoothLineStep(originPoint, destinationPoint, halfdistX, halfdistY, controlPoint1, this->originItem()->getConnectionDirection());
	this->calculateSmoothLineStep(destinationPoint, originPoint, halfdistX, halfdistY, controlPoint2, this->destItem()->getConnectionDirection());

	return Math::calculateShortestDistanceFromPointToBezierCurve(_pt.x(), _pt.y(), originPoint.x(), originPoint.y(), controlPoint1.x(), controlPoint1.y(), controlPoint2.x(), controlPoint2.y(), destinationPoint.x(), destinationPoint.y());
}

qreal ot::GraphicsConnectionItem::calculateShortestDistanceToPointXY(const QPointF& _pt) const {
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Origin and/or destination not set");
		return -1.;
	}

	QPointF originPoint = this->originItem()->getQGraphicsItem()->mapToScene(this->originItem()->getQGraphicsItem()->boundingRect().center());
	QPointF destinationPoint = this->destItem()->getQGraphicsItem()->mapToScene(this->destItem()->getQGraphicsItem()->boundingRect().center());
	QPointF controlPoint(destinationPoint.x(), originPoint.y());

	qreal d1 = Math::calculateShortestDistanceFromPointToLine(_pt.x(), _pt.y(), originPoint.x(), originPoint.y(), controlPoint.x(), controlPoint.y());
	qreal d2 = Math::calculateShortestDistanceFromPointToLine(_pt.x(), _pt.y(), controlPoint.x(), controlPoint.y(), destinationPoint.x(), destinationPoint.y());

	return std::min(d1, d2);
}

qreal ot::GraphicsConnectionItem::calculateShortestDistanceToPointYX(const QPointF& _pt) const {
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Origin and/or destination not set");
		return -1.;
	}

	QPointF originPoint = this->originItem()->getQGraphicsItem()->mapToScene(this->originItem()->getQGraphicsItem()->boundingRect().center());
	QPointF destinationPoint = this->destItem()->getQGraphicsItem()->mapToScene(this->destItem()->getQGraphicsItem()->boundingRect().center());
	QPointF controlPoint(originPoint.x(), destinationPoint.y());

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

ot::GraphicsConnectionCfg::ConnectionShape ot::GraphicsConnectionItem::calculateAutoXYShape(void) const
{
	OTAssertNullptr(m_origin);
	OTAssertNullptr(m_dest);

	Alignment connectionAlignment = this->calculateConnectionDirectionAlignment();
	
	if (
		connectionAlignment == AlignLeft ||
		connectionAlignment == AlignTop ||
		connectionAlignment == AlignRight ||
		connectionAlignment == AlignBottom ||
		connectionAlignment == AlignCenter
		) {
		return GraphicsConnectionCfg::ConnectionShape::XYLine;
	}

	GraphicsConnectionCalculationData info(m_origin->getConnectionDirection(), m_dest->getConnectionDirection(), connectionAlignment);
	std::list<GraphicsConnectionCalculationData> yxLines = GraphicsConnectionCalculationData::getAllYXLines();
	
	if (std::find(yxLines.begin(), yxLines.end(), info) != yxLines.end()) {
		return GraphicsConnectionCfg::ConnectionShape::YXLine;
	}
	else {
		return GraphicsConnectionCfg::ConnectionShape::XYLine;
	}
}

ot::Alignment ot::GraphicsConnectionItem::calculateConnectionDirectionAlignment(void) const {
	OTAssertNullptr(m_origin);
	OTAssertNullptr(m_dest);

	QPointF originPoint = this->originItem()->getQGraphicsItem()->mapToScene(this->originItem()->getQGraphicsItem()->boundingRect().center());
	QPointF destinationPoint = this->destItem()->getQGraphicsItem()->mapToScene(this->destItem()->getQGraphicsItem()->boundingRect().center());
	QPointF delta = destinationPoint - originPoint;

	if (delta.x() > 0.) {
		if (delta.y() > 0.) return Alignment::AlignBottomRight;
		else if (delta.y() < 0.) return Alignment::AlignTopRight;
		else return Alignment::AlignRight;
	}
	else if (delta.x() < 0.) {
		if (delta.y() > 0.) return Alignment::AlignBottomLeft;
		else if (delta.y() < 0.) return Alignment::AlignTopLeft;
		else return Alignment::AlignLeft;
	}
	else {
		if (delta.y() > 0.) return Alignment::AlignBottom;
		else if (delta.y() < 0.) return Alignment::AlignTop;
		else return Alignment::AlignCenter;
	}
}
