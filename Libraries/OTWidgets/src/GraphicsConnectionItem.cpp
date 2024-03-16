//! @file GraphicsConnectionItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/OTQtConverter.h"

// Qt header
#include <QtGui/qpainter.h>

ot::GraphicsConnectionItem::GraphicsConnectionItem()
	: m_dest(nullptr), m_origin(nullptr), m_style(ot::GraphicsConnectionCfg::DirectLine), m_state(NoState), m_uid(0)
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

	switch (m_style)
	{
	case ot::GraphicsConnectionCfg::DirectLine:
	{
		QPointF orig;
		QPointF dest;
		this->calculateDirectLinePoints(orig, dest);
		return QRectF(QPointF(std::min(orig.x(), dest.x()), std::min(orig.y(), dest.y())), QPointF(std::max(orig.x(), dest.x()), std::max(orig.y(), dest.y())));
	}
	case ot::GraphicsConnectionCfg::SmoothLine:
	{
		QPointF orig;
		QPointF c1;
		QPointF c2;
		QPointF dest;
		this->calculateSmoothLinePoints(orig, c1, c2, dest);
		return QRectF(QPointF(std::min({ orig.x(), c1.x(), c2.x(), dest.x() }), std::min({ orig.y(), c1.y(), c2.y(), dest.y() })),
			QPointF(std::max({ orig.x(), c1.x(), c2.x(), dest.x() }), std::max({ orig.y(), c1.y(), c2.y(), dest.y() })));
	}
	break;
	default:
		OT_LOG_EA("Unknown connection style");
	}

	return m_lastRect;
}

void ot::GraphicsConnectionItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	switch (m_style)
	{
	case ot::GraphicsConnectionCfg::DirectLine:
	{
		QPointF orig;
		QPointF dest;
		this->calculateDirectLinePoints(orig, dest);

		if (m_state & HoverState) {
			QPen p(QBrush(QColor(0, 0, 255)), m_pen.widthF() + 2.f);
			_painter->setPen(p);
			_painter->drawLine(orig, dest);
		}
		else if (m_state & SelectedState) {
			QPen p(QBrush(QColor(255, 255, 0)), m_pen.widthF() + 2.f);
			_painter->setPen(p);
			_painter->drawLine(orig, dest);
		}

		_painter->setPen(m_pen);
		_painter->drawLine(orig, dest);
	}
		break;
	case ot::GraphicsConnectionCfg::SmoothLine:
	{
		QPointF orig;
		QPointF c1;
		QPointF c2;
		QPointF dest;
		this->calculateSmoothLinePoints(orig, c1, c2, dest);
		QPainterPath path(orig);
		path.cubicTo(c1, c2, dest);

		if (m_state & HoverState) {
			QPen p(QBrush(QColor(0, 0, 255)), m_pen.widthF() + 2.f);
			_painter->setPen(p);
			_painter->drawPath(path);
		}
		else if (m_state & SelectedState) {
			QPen p(QBrush(QColor(255, 255, 0)), m_pen.widthF() + 2.f);
			_painter->setPen(p);
			_painter->drawPath(path);
		}

		_painter->setPen(m_pen);
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
	this->setSelected(true);
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

bool ot::GraphicsConnectionItem::setupFromConfig(const ot::GraphicsConnectionCfg& _cfg) {
	this->m_uid = _cfg.getUid();
	this->m_style = _cfg.style();
	m_pen.setWidth(_cfg.lineWidth());
	m_pen.setColor(ot::OTQtConverter::toQt(_cfg.color()));

	m_connectionKey = _cfg.createConnectionKey();
	return true;
}

ot::GraphicsConnectionCfg ot::GraphicsConnectionItem::getConnectionInformation(void) const {
	ot::GraphicsConnectionCfg info;
	info.setUid(m_uid);

	if (m_origin) {
		info.setOriginUid(m_origin->getRootItem()->graphicsItemUid());
		info.setOriginConnectable(m_origin->graphicsItemName());
	}
	else {
		OT_LOG_WA("No origin item set");
	}
	if (m_dest) {
		info.setDestUid(m_dest->getRootItem()->graphicsItemUid());
		info.setDestConnectable(m_dest->graphicsItemName());
	}
	else {
		OT_LOG_WA("No dest item set");
	}
	return info;
}

void ot::GraphicsConnectionItem::connectItems(GraphicsItem* _origin, GraphicsItem* _dest) {
	OTAssertNullptr(_origin);
	OTAssertNullptr(_dest);
	OTAssert(m_origin == nullptr, "Origin already set");
	OTAssert(m_dest == nullptr, "Destination already set");
	m_origin = _origin;
	m_dest = _dest;
	m_origin->storeConnection(this);
	m_dest->storeConnection(this);
	this->updateConnection();
}

void ot::GraphicsConnectionItem::disconnectItems(void) {
	m_lastRect = this->boundingRect();

	if (m_origin) {
		m_origin->forgetConnection(this);
		m_origin = nullptr;
	}
	if (m_dest) {
		m_dest->forgetConnection(this);
		m_dest = nullptr;
	}
}

void ot::GraphicsConnectionItem::updateConnection(void) {
	if (m_origin && m_dest) {
		this->prepareGeometryChange();

		//this->update(this->boundingRect());
		this->update();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

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
	this->calculateSmoothLineStep(_origin, _destination, halfdistX, halfdistY, _control1, this->originItem()->connectionDirection());
	this->calculateSmoothLineStep(_destination, _origin, halfdistX, halfdistY, _control2, this->destItem()->connectionDirection());
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
