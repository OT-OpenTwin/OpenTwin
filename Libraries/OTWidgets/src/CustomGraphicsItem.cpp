// @otlicense

// OpenTwin header
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/CustomGraphicsItem.h"

// Qt header
#include <QtGui/qpainter.h>

ot::CustomGraphicsItem::CustomGraphicsItem(GraphicsItemCfg* _configuration)
	: ot::GraphicsItem(_configuration)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setGraphicsItem(this);
	this->setAcceptHoverEvents(true);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::CustomGraphicsItem::~CustomGraphicsItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// ot::GraphicsItem

bool ot::CustomGraphicsItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	if (!ot::GraphicsItem::setupFromConfig(_cfg)) return false;

	return true;
	
}

void ot::CustomGraphicsItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

void ot::CustomGraphicsItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

QSizeF ot::CustomGraphicsItem::graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->sizeHint(_hint, _constrains);
}

// ###########################################################################################################################################################################################################################################################################################################################

// QGraphicsLayoutItem

QSizeF ot::CustomGraphicsItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->handleGetGraphicsItemSizeHint(_hint, this->getPreferredGraphicsItemSize());
}

void ot::CustomGraphicsItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setGraphicsItemPos(_rect.topLeft());
	this->handleSetItemGeometry(_rect);
}

// ###########################################################################################################################################################################################################################################################################################################################

// QGraphicsItem

QRectF ot::CustomGraphicsItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QRectF(QPointF(0., 0.), QSizeF(this->getPreferredGraphicsItemSize())));
}

void ot::CustomGraphicsItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintCustomItem(_painter, _opt, _widget, this->calculatePaintArea(this->getPreferredGraphicsItemSize()));
}

QVariant ot::CustomGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return QGraphicsItem::itemChange(_change, _value);
}

void ot::CustomGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	this->handleMousePressEvent(_event);
	QGraphicsItem::mousePressEvent(_event);
}

void ot::CustomGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* _event) {
	if (this->handleMouseMoveEvent(_event)) {
		QGraphicsItem::mouseMoveEvent(_event);
	}
}

void ot::CustomGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	this->handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::CustomGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverEnterEvent(_event);
}

void ot::CustomGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverLeaveEvent(_event);
}

void ot::CustomGraphicsItem::graphicsElementStateChanged(const GraphicsElementStateFlags& _state) {
	GraphicsItem::graphicsElementStateChanged(_state);
}

void ot::CustomGraphicsItem::updateItemGeometry(void) {
	this->prepareGeometryChange();
	this->setGeometry(QRectF(this->pos(), this->getPreferredGraphicsItemSize()));
	this->updateGeometry();
}