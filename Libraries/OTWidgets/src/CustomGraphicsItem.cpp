//! @file CustomGraphicsItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/CustomGraphicsItem.h"

ot::CustomGraphicsItem::CustomGraphicsItem() {

}

ot::CustomGraphicsItem::~CustomGraphicsItem() {

}

bool ot::CustomGraphicsItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::CustomGraphicsItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

void ot::CustomGraphicsItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::CustomGraphicsItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

QSizeF ot::CustomGraphicsItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->handleGetGraphicsItemSizeHint(_hint, this->getDefaultGraphicsItemSize());
}

void ot::CustomGraphicsItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());

	this->handleSetItemGeometry(_rect);
}

QRectF ot::CustomGraphicsItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QRectF(QPointF(0., 0.), QSizeF(this->getDefaultGraphicsItemSize())));
}

void ot::CustomGraphicsItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	this->paintCustomItem(_painter, _opt, _widget, this->calculatePaintArea(this->getDefaultGraphicsItemSize()));
}

QVariant ot::CustomGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return _value;
}

void ot::CustomGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::CustomGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}
