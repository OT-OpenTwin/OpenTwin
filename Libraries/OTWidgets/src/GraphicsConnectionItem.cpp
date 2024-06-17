//! @file GraphicsConnectionItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsScene.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtWidgets/qgraphicssceneevent.h>

ot::GraphicsConnectionItem::GraphicsConnectionItem()
	: m_dest(nullptr), m_origin(nullptr), m_state(NoState)
{
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);
	this->setAcceptHoverEvents(true);
}

ot::GraphicsConnectionItem::~GraphicsConnectionItem() {
	this->disconnectItems();
}

// ###########################################################################################################################################################################################################################################################################################################################


QRectF ot::GraphicsConnectionItem::boundingRect(void) const {
	if (m_dest == nullptr || m_origin == nullptr) {
		return m_lastRect;
	}

	switch (m_config.getLineShape())
	{
	case ot::GraphicsConnectionCfg::ConnectionShape::DirectLine:
	{
		QPointF orig;
		QPointF dest;
		this->calculateDirectLinePoints(orig, dest);
		qreal marg = m_config.getLineStyle().width();
		return QRectF(
			QPointF(std::min(orig.x(), dest.x()), std::min(orig.y(), dest.y())), 
			QPointF(std::max(orig.x(), dest.x()), std::max(orig.y(), dest.y()))).marginsAdded(QMarginsF(marg, marg, marg, marg));
	}
	case ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine:
	{
		QPointF orig;
		QPointF c1;
		QPointF c2;
		QPointF dest;
		this->calculateSmoothLinePoints(orig, c1, c2, dest);
		qreal marg = m_config.getLineStyle().width();
		return QRectF(QPointF(std::min({ orig.x(), c1.x(), c2.x(), dest.x() }), std::min({ orig.y(), c1.y(), c2.y(), dest.y() })),
			QPointF(std::max({ orig.x(), c1.x(), c2.x(), dest.x() }), std::max({ orig.y(), c1.y(), c2.y(), dest.y() }))).marginsAdded(QMarginsF(marg, marg, marg, marg));
	}
	break;
	default:
		OT_LOG_EA("Unknown connection style");
	}

	return m_lastRect;
}

void ot::GraphicsConnectionItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {

	QPen linePen = QtFactory::toQPen(m_config.getLineStyle());

	if (m_state & GraphicsConnectionItem::HoverState) {
		Painter2D* newPainter = GraphicsItem::createHoverBorderPainter();
		linePen.setBrush(QtFactory::toQBrush(newPainter));
		delete newPainter;
	}
	else if (m_state & GraphicsConnectionItem::SelectedState) {
		Painter2D* newPainter = GraphicsItem::createSelectionBorderPainter();
		linePen.setBrush(QtFactory::toQBrush(newPainter));
		delete newPainter;
	}

	_painter->setPen(linePen);

	switch (m_config.getLineShape())
	{
	case ot::GraphicsConnectionCfg::ConnectionShape::DirectLine:
	{
		QPointF orig;
		QPointF dest;
		this->calculateDirectLinePoints(orig, dest);
		_painter->drawLine(orig, dest);
	}
		break;
	case ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine:
	{
		QPointF orig;
		QPointF c1;
		QPointF c2;
		QPointF dest;
		this->calculateSmoothLinePoints(orig, c1, c2, dest);
		QPainterPath path(orig);
		path.cubicTo(c1, c2, dest);

		_painter->drawPath(path);
	}
		break;
	default:
		OT_LOG_EA("Unknown connection style");
		break;
	}
}

QVariant ot::GraphicsConnectionItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	switch (_change)
	{
	case QGraphicsItem::ItemSelectedHasChanged:
		if (this->isSelected()) {
			m_state |= SelectedState;
		}
		else {
			m_state &= (~SelectedState);
		}
		break;
	default:
		break;
	}
	return QGraphicsItem::itemChange(_change, _value);
}

void ot::GraphicsConnectionItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	if (_event->button() == Qt::LeftButton) {
		GraphicsScene* sc = dynamic_cast<GraphicsScene*>(this->scene());
		if (sc) {
			if (_event->modifiers() != Qt::ControlModifier) {
				sc->setIgnoreEvents(true);
				sc->clearSelection();
				sc->setIgnoreEvents(false);
			}
			this->setSelected(true);
			this->update();

			sc->handleSelectionChanged();
		}
		else {
			OT_LOG_EA("Scene cast failed");
		}
		
	}
}

void ot::GraphicsConnectionItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {

}

void ot::GraphicsConnectionItem::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	m_state |= HoverState;
	this->update(this->boundingRect());
}

void ot::GraphicsConnectionItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	m_state &= (~HoverState);
	this->update(this->boundingRect());
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
	this->updateConnection();
}

void ot::GraphicsConnectionItem::disconnectItems(void) {
	this->prepareGeometryChange();

	m_lastRect = this->boundingRect();

	if (m_origin) {
		m_origin->forgetConnection(this);
		m_origin = nullptr;
	}
	if (m_dest) {
		m_dest->forgetConnection(this);
		m_dest = nullptr;
	}

	this->updateConnectionInformation();
}

void ot::GraphicsConnectionItem::updateConnection(void) {
	if (m_origin && m_dest) {		
		this->update();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

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
		m_config.setOriginConnectable("");
		m_config.setOriginUid(0);
	}
}

void ot::GraphicsConnectionItem::calculateDirectLinePoints(QPointF& _origin, QPointF& _destination) const {
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Origin and/or destination not set");
		return;
	}

	_origin = this->originItem()->getQGraphicsItem()->scenePos() + this->originItem()->getQGraphicsItem()->boundingRect().center();
	_destination = this->destItem()->getQGraphicsItem()->scenePos() + this->destItem()->getQGraphicsItem()->boundingRect().center();
}

void ot::GraphicsConnectionItem::calculateSmoothLinePoints(QPointF& _origin, QPointF& _control1, QPointF& _control2, QPointF& _destination) const {
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Origin and/or destination not set");
		return;
	}

	// Get the center point of the connectable items
	_origin = this->originItem()->getQGraphicsItem()->scenePos() + this->originItem()->getQGraphicsItem()->boundingRect().center();
	_destination = this->destItem()->getQGraphicsItem()->scenePos() + this->destItem()->getQGraphicsItem()->boundingRect().center();

	// Calculate distance between the items
	double halfdistX = (std::max(_origin.x(), _destination.x()) - std::min(_origin.x(), _destination.x())) / 2.;
	double halfdistY = (std::max(_origin.y(), _destination.y()) - std::min(_origin.y(), _destination.y())) / 2.;

	// Calculate control points
	this->calculateSmoothLineStep(_origin, _destination, halfdistX, halfdistY, _control1, this->originItem()->getConnectionDirection());
	this->calculateSmoothLineStep(_destination, _origin, halfdistX, halfdistY, _control2, this->destItem()->getConnectionDirection());
}

void ot::GraphicsConnectionItem::calculateSmoothLineStep(const QPointF& _origin, const QPointF& _destination, double _halfdistX, double _halfdistY, QPointF& _control, ot::ConnectionDirection _direction) const {	switch (_direction)
	{
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
