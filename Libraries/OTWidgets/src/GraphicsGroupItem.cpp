//! @file GraphicsGroupItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsGroupItem.h"

ot::GraphicsGroupItem::GraphicsGroupItem(bool _isStackOrLayout)
	: ot::GraphicsItem(_isStackOrLayout)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsGroupItem::~GraphicsGroupItem() {

}

bool ot::GraphicsGroupItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsGroupItem::removeAllConnections(void) {
	ot::GraphicsItem::removeAllConnections();
}

void ot::GraphicsGroupItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

QSizeF ot::GraphicsGroupItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->handleGetGraphicsItemSizeHint(_hint, this->boundingRect().size());
}

QRectF ot::GraphicsGroupItem::boundingRect(void) const {
	//return QGraphicsItemGroup::boundingRect();
	return this->handleGetGraphicsItemBoundingRect(QGraphicsItemGroup::boundingRect());
}

void ot::GraphicsGroupItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	QGraphicsLayoutItem::setGeometry(_rect);
	this->setPos(_rect.topLeft());
}

QVariant ot::GraphicsGroupItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return QGraphicsItemGroup::itemChange(_change, _value);
}

void ot::GraphicsGroupItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItemGroup::mousePressEvent(_event);
}

void ot::GraphicsGroupItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItem::mouseReleaseEvent(_event);
}

void ot::GraphicsGroupItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsGroupItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	QGraphicsItemGroup::paint(_painter, _opt, _widget);
}

void ot::GraphicsGroupItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

ot::GraphicsItem* ot::GraphicsGroupItem::findItem(const std::string& _itemName) {
	if (_itemName == this->graphicsItemName()) return this;

	for (auto i : this->childItems()) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(i);
		if (itm) {
			auto r = itm->findItem(_itemName);
			if (r) return r;
		}
	}
	return nullptr;
}

QSizeF ot::GraphicsGroupItem::graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->sizeHint(_hint, _constrains);
}