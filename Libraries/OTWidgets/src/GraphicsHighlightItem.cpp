//! @file GraphicsHighlightItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTWidgets/GraphicsHighlightItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

ot::GraphicsHighlightItem::GraphicsHighlightItem()
{
	this->setRectangleSize(QSizeF(10., 10.));
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
}

ot::GraphicsHighlightItem::~GraphicsHighlightItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsHighlightItem

void ot::GraphicsHighlightItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (this->getStateFlags() & HoverState) {
		QPen p(QBrush(QColor(0, 0, 255)), 2.f);
		_painter->setPen(p);
		_painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
		_painter->drawRect(this->boundingRect());
	}
	else if (this->getStateFlags() & SelectedState) {
		QPen p(QBrush(QColor(0, 255, 0)), 2.f);
		_painter->setPen(p);
		_painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
		_painter->drawRect(this->boundingRect());
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// QGraphicsItem

QVariant ot::GraphicsHighlightItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	return GraphicsRectangularItem::itemChange(_change, _value);
}


void ot::GraphicsHighlightItem::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverEnterEvent(_event);
}

void ot::GraphicsHighlightItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverLeaveEvent(_event);
}