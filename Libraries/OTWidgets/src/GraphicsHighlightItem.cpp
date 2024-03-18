//! @file GraphicsHighlightItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsHighlightItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

ot::GraphicsHighlightItem::GraphicsHighlightItem()
	: ot::GraphicsItem(false), m_size(10, 10)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setGraphicsItem(this);
}

ot::GraphicsHighlightItem::~GraphicsHighlightItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsHighlightItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsHighlightItem

void ot::GraphicsHighlightItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (this->stateFlags() & HoverState) {
		QPen p(QBrush(QColor(0, 0, 255)), 2.f);
		_painter->setPen(p);
		_painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
		_painter->drawRect(this->boundingRect());
	}
	else if (this->stateFlags() & SelectedState) {
		QPen p(QBrush(QColor(0, 255, 0)), 2.f);
		_painter->setPen(p);
		_painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
		_painter->drawRect(this->boundingRect());
	}
}

void ot::GraphicsHighlightItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

void ot::GraphicsHighlightItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsHighlightItem::graphicsItemFlagsChanged(GraphicsItemCfg::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & GraphicsItemCfg::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & GraphicsItemCfg::ItemIsMoveable);
}

QSizeF ot::GraphicsHighlightItem::graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->sizeHint(_hint, _constrains);
}

// ###########################################################################################################################################################################################################################################################################################################################

// QGraphicsLayoutItem

QSizeF ot::GraphicsHighlightItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->handleGetGraphicsItemSizeHint(_hint, m_size);
}

void ot::GraphicsHighlightItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
	this->handleSetItemGeometry(_rect);
}

// ###########################################################################################################################################################################################################################################################################################################################

// QGraphicsItem

QRectF ot::GraphicsHighlightItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QRectF(QPointF(0., 0.), m_size));
}

QVariant ot::GraphicsHighlightItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return QGraphicsItem::itemChange(_change, _value);
}

void ot::GraphicsHighlightItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	this->handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::GraphicsHighlightItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	this->handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsHighlightItem::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverEnterEvent(_event);
}

void ot::GraphicsHighlightItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverLeaveEvent(_event);
}
