//! @file GraphicsGroupItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsGroupItemCfg.h"
#include "OTWidgets/GraphicsGroupItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsGroupItem> groupItemRegistrar(OT_FactoryKey_GraphicsGroupItem);

ot::GraphicsGroupItem::GraphicsGroupItem()
	: ot::GraphicsItem(new GraphicsGroupItemCfg)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsGroupItem::~GraphicsGroupItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: GraphicsItem

bool ot::GraphicsGroupItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	const GraphicsGroupItemCfg* cfg = dynamic_cast<const GraphicsGroupItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	for (GraphicsItemCfg* i : cfg->getItems()) {
		OTAssertNullptr(i);

		ot::GraphicsItem* itm = ot::GraphicsItemFactory::instance().itemFromConfig(i);
		if (itm) {
			this->addItem(itm);
		}
	}

	return GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsGroupItem::removeAllConnections(void) {
	ot::GraphicsItem::removeAllConnections();
}

void ot::GraphicsGroupItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

void ot::GraphicsGroupItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsGroupItem::graphicsItemFlagsChanged(const GraphicsItemCfg::GraphicsItemFlags& _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & GraphicsItemCfg::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & GraphicsItemCfg::ItemIsMoveable);
}

QSizeF ot::GraphicsGroupItem::graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->sizeHint(_hint, _constrains);
}

ot::GraphicsItem* ot::GraphicsGroupItem::findItem(const std::string& _itemName) {
	if (_itemName == this->getGraphicsItemName()) return this;

	for (auto i : this->childItems()) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(i);
		if (itm) {
			auto r = itm->findItem(_itemName);
			if (r) return r;
		}
		else {
			OT_LOG_EA("Item cast failed");
		}
	}
	return nullptr;
}

void ot::GraphicsGroupItem::finalizeGraphicsItem(void) {
	for (auto i : this->childItems()) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(i);
		if (itm) {
			itm->finalizeGraphicsItem();
		}
		else {
			OT_LOG_EA("Item cast failed");
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: QGraphicsItem

QRectF ot::GraphicsGroupItem::boundingRect(void) const {
	return this->handleGetGraphicsItemBoundingRect(QGraphicsItemGroup::boundingRect());
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

void ot::GraphicsGroupItem::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverEnterEvent(_event);
}

void ot::GraphicsGroupItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverLeaveEvent(_event);
}

void ot::GraphicsGroupItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paintStateBackground(_painter, _opt, _widget);
	QGraphicsItemGroup::paint(_painter, _opt, _widget);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: QGraphicsLayoutItem

QSizeF ot::GraphicsGroupItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->handleGetGraphicsItemSizeHint(_hint, this->boundingRect().size());
}

void ot::GraphicsGroupItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	this->setGraphicsItemPos(_rect.topLeft());
	this->handleSetItemGeometry(_rect);
	QGraphicsLayoutItem::setGeometry(_rect);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsGroupItem::addItem(GraphicsItem* _item) {
	OTAssertNullptr(_item);

	_item->setParentGraphicsItem(this);

	this->addToGroup(_item->getQGraphicsItem());
}
