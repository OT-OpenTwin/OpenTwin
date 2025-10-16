//! @file GraphicsGroupItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsGroupItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsGroupItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsGroupItem> groupItemRegistrar(ot::GraphicsGroupItemCfg::className());

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

	this->setBlockConfigurationNotifications(true);
	for (GraphicsItemCfg* i : cfg->getItems()) {
		OTAssertNullptr(i);

		ot::GraphicsItem* itm = ot::GraphicsItemFactory::itemFromConfig(i);
		if (itm) {
			this->addItem(itm);
		}
	}
	this->setBlockConfigurationNotifications(false);

	return GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsGroupItem::removeAllConnections(void) {
	for (QGraphicsItem* itm : this->childItems()) {
		GraphicsItem* actualItem = dynamic_cast<GraphicsItem*>(itm);
		if (actualItem) actualItem->removeAllConnections();
	}
	ot::GraphicsItem::removeAllConnections();
}

void ot::GraphicsGroupItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

void ot::GraphicsGroupItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
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

std::list<ot::GraphicsConnectionItem*> ot::GraphicsGroupItem::getAllConnections() const {
	std::list<GraphicsConnectionItem*> lst = GraphicsItem::getAllConnections();

	for (QGraphicsItem* child : this->childItems()) {
		GraphicsItem* actualItem = dynamic_cast<GraphicsItem*>(child);
		if (actualItem) {
			lst.splice(lst.end(), actualItem->getAllConnections());
		}
	}

	return lst;
}

std::list<ot::GraphicsElement*> ot::GraphicsGroupItem::getAllGraphicsElements(void) {
	std::list<GraphicsElement*> result = GraphicsItem::getAllGraphicsElements();
	for (QGraphicsItem* child : this->childItems()) {
		GraphicsElement* actualItem = dynamic_cast<GraphicsElement*>(child);
		if (actualItem) {
			result.splice(result.end(), actualItem->getAllGraphicsElements());
		}
		else {
			OT_LOG_EA("Unknown item");
		}
	}
	return result;
}

std::list<ot::GraphicsElement*> ot::GraphicsGroupItem::getAllDirectChildElements(void) {
	std::list<GraphicsElement*> result;
	for (QGraphicsItem* child : this->childItems()) {
		GraphicsElement* actualItem = dynamic_cast<GraphicsElement*>(child);
		if (actualItem) {
			result.push_back(actualItem);
		}
		else {
			OT_LOG_EA("Unknown item");
		}
	}
	return result;
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
	this->handleMousePressEvent(_event);
	QGraphicsItemGroup::mousePressEvent(_event);
}

void ot::GraphicsGroupItem::mouseMoveEvent(QGraphicsSceneMouseEvent* _event) {
	if (this->handleMouseMoveEvent(_event)) {
		QGraphicsItemGroup::mouseMoveEvent(_event);
	}
}

void ot::GraphicsGroupItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	this->handleMouseReleaseEvent(_event);
	QGraphicsItemGroup::mouseReleaseEvent(_event);
}

void ot::GraphicsGroupItem::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverEnterEvent(_event);
}

void ot::GraphicsGroupItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverLeaveEvent(_event);
}

void ot::GraphicsGroupItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	// Manually paint the grouped items and DON'T call the Qt paint implementation to avoid the selection border from being painted
	for (QGraphicsItem* child : childItems()) {
		QGraphicsItem* childItem = static_cast<QGraphicsItem*>(child);
		_painter->save();
		_painter->translate(childItem->pos());
		childItem->paint(_painter, _opt, _widget);
		_painter->restore();
	}
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

double ot::GraphicsGroupItem::getMaxAdditionalTriggerDistance(void) const {
	double maxDist = GraphicsItem::getMaxAdditionalTriggerDistance();
	for (auto itm : this->childItems()) {
		const GraphicsItem* actualItem = dynamic_cast<const GraphicsItem*>(itm);
		if (actualItem) {
			maxDist = std::max(maxDist, actualItem->getMaxAdditionalTriggerDistance());
		}
	}
	return maxDist;
}

void ot::GraphicsGroupItem::clear(void) {
	for (auto itm : this->childItems()) {
		this->removeFromGroup(itm);
		delete itm;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::GraphicsGroupItem::graphicsElementStateChanged(const GraphicsElementStateFlags& _state) {
	if (this->getGraphicsItemFlags() & GraphicsItemCfg::GraphicsItemFlag::ItemForwardsState) {
		for (auto i : this->childItems()) {
			ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(i);
			if (itm) {
				itm->setGraphicsElementStateFlags(_state);
			}
			else {
				OT_LOG_EA("Item cast failed");
			}
		}
	}

	this->update();
}

void ot::GraphicsGroupItem::notifyChildsAboutTransformChange(const QTransform& _newTransform) {
	for (auto i : this->childItems()) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(i);
		if (itm) {
			itm->parentItemTransformChanged(_newTransform);
		}
		else {
			OT_LOG_EA("Item cast failed");
		}
	}
}