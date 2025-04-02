//! @file GraphicsStackItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsStackItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsStackItem> stackItemRegistrar(OT_FactoryKey_GraphicsStackItem);

ot::GraphicsStackItem::GraphicsStackItem() 
	: ot::GraphicsItem(new GraphicsStackItemCfg), m_lastCalculatedSize(-1., -1.) 
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
	this->setForwardSizeChanges(true);
}

ot::GraphicsStackItem::~GraphicsStackItem() {
	for (GraphicsStackItemEntry& entry : m_items) {
		entry.item->setParentGraphicsItem(nullptr);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: GraphicsItem

bool ot::GraphicsStackItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);

	const GraphicsStackItemCfg* cfg = dynamic_cast<const GraphicsStackItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->setBlockConfigurationNotifications(true);
	this->clear();

	for (auto itm : cfg->items()) {
		OTAssertNullptr(itm.item);

		ot::GraphicsItem* i = nullptr;
		try {
			i = ot::GraphicsItemFactory::itemFromConfig(itm.item);
			if (i) {
				this->addItem(i, itm.isMaster, itm.isSlave);
			}
			else {
				OT_LOG_EA("Failed to created graphics item from factory");
			}
		}
		catch (const std::exception& _e) {
			OT_LOG_EAS("Failed to create child item: " + std::string(_e.what()));
			if (i) delete i;
			this->setBlockConfigurationNotifications(false);
			throw _e;
		}
	}

	this->setBlockConfigurationNotifications(false);
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsStackItem::removeAllConnections(void) {
	ot::GraphicsItem::removeAllConnections();
	for (auto itm : m_items) {
		itm.item->removeAllConnections();
	}
}

void ot::GraphicsStackItem::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

void ot::GraphicsStackItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsStackItem::graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) {
	if (_event == ot::GraphicsItem::ItemResized) {
		this->adjustChildItems();
	}
}

void ot::GraphicsStackItem::setGraphicsItemRequestedSize(const QSizeF& _size) {
	ot::GraphicsItem::setGraphicsItemRequestedSize(_size);
	for (auto itm : m_items) {
		itm.item->setGraphicsItemRequestedSize(_size);
	}
}

QSizeF ot::GraphicsStackItem::graphicsItemSizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	return this->sizeHint(_hint, _constrains); 
}

ot::GraphicsItem* ot::GraphicsStackItem::findItem(const std::string& _itemName) {
	if (_itemName == this->getGraphicsItemName()) return this;

	for (QGraphicsItem* i : this->childItems()) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(i);
		if (itm) {
			auto r = itm->findItem(_itemName);
			if (r) return r;
		}
	}
	return nullptr;
}

void ot::GraphicsStackItem::finalizeGraphicsItem(void) {
	for (const GraphicsStackItemEntry& entry : m_items) {
		entry.item->finalizeGraphicsItem();
	}
}

std::list<ot::GraphicsElement*> ot::GraphicsStackItem::getAllGraphicsElements(void)
{
	std::list<GraphicsElement*> result = GraphicsItem::getAllGraphicsElements();
	for (GraphicsStackItemEntry child : m_items) {
		result.splice(result.end(), child.item->getAllGraphicsElements());
	}
	return result;
}

std::list<ot::GraphicsElement*> ot::GraphicsStackItem::getAllDirectChildElements(void) {
	std::list<GraphicsElement*> result;
	for (GraphicsStackItemEntry child : m_items) {
		result.push_back(child.item);
	}
	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: QGraphicsItem

void ot::GraphicsStackItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (this->adjustChildItems()) return;

	// Manually paint the grouped items and DON'T call the Qt paint implementation to avoid the selection border from being painted
	for (QGraphicsItem* child : childItems()) {
		QGraphicsItem* childItem = static_cast<QGraphicsItem*>(child);
		_painter->save();
		_painter->translate(childItem->pos());
		childItem->paint(_painter, _opt, _widget);
		_painter->restore();
	}
}

QVariant ot::GraphicsStackItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	this->handleItemChange(_change, _value);
	return QGraphicsItemGroup::itemChange(_change, _value);
}

QRectF ot::GraphicsStackItem::boundingRect(void) const {
	QRectF rec;

	for (auto itm : m_items) {
		if (rec.isValid()) {
			rec = itm.item->getQGraphicsItem()->boundingRect().united(rec);
		}
		else {
			rec = itm.item->getQGraphicsItem()->boundingRect();
		}
	}
	return this->handleGetGraphicsItemBoundingRect(rec);
}

void ot::GraphicsStackItem::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMousePressEvent(_event);
	QGraphicsItemGroup::mousePressEvent(_event);
}

void ot::GraphicsStackItem::mouseMoveEvent(QGraphicsSceneMouseEvent* _event) {
	if (this->handleMouseMoveEvent(_event)) {
		QGraphicsItemGroup::mouseMoveEvent(_event);
	}
}

void ot::GraphicsStackItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	GraphicsItem::handleMouseReleaseEvent(_event);
	QGraphicsItemGroup::mouseReleaseEvent(_event);
}

void ot::GraphicsStackItem::hoverEnterEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverEnterEvent(_event);
}

void ot::GraphicsStackItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* _event) {
	this->handleHoverLeaveEvent(_event);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: QGraphicsLayoutItem

QSizeF ot::GraphicsStackItem::sizeHint(Qt::SizeHint _hint, const QSizeF& _constrains) const {
	QSizeF s;
	for (auto itm : m_items) {
		if (itm.isMaster) {
			s = s.expandedTo(itm.item->graphicsItemSizeHint(_hint, _constrains));
		}
		else {
			s = s.expandedTo(itm.item->graphicsItemSizeHint(Qt::MinimumSize, _constrains));
		}
	}
	return this->handleGetGraphicsItemSizeHint(_hint, s);
}

void ot::GraphicsStackItem::setGeometry(const QRectF& _rect) {
	this->prepareGeometryChange();
	this->setGraphicsItemPos(_rect.topLeft());
	this->handleSetItemGeometry(_rect);
	QGraphicsLayoutItem::setGeometry(_rect);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsStackItem::addItem(ot::GraphicsItem* _item, bool _isMaster, bool _isSlave) {
	GraphicsStackItemEntry e;
	e.isMaster = _isMaster;
	e.isSlave = _isSlave;
	e.item = _item;
	_item->setParentGraphicsItem(this);
	if (_isMaster) {
		// If the item is a master item, install an event filter for resizing the child items
		_item->addGraphicsItemEventHandler(this);
	}
	m_items.push_back(e);
	this->addToGroup(e.item->getQGraphicsItem());
}

double ot::GraphicsStackItem::getMaxAdditionalTriggerDistance(void) const {
	double maxDist = ot::GraphicsItem::getMaxAdditionalTriggerDistance();
	for (auto itm : m_items) {
		if (itm.item) {
			maxDist = std::max(maxDist, itm.item->getMaxAdditionalTriggerDistance());
		}
	}
	return maxDist;
}

void ot::GraphicsStackItem::clear(void) {
	for (auto itm : m_items) {
		delete itm.item;
	}
	m_items.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::GraphicsStackItem::graphicsElementStateChanged(const GraphicsElementStateFlags& _state) {
	if (this->getGraphicsItemFlags() & GraphicsItemCfg::GraphicsItemFlag::ItemForwardsState) {
		for (const GraphicsStackItemEntry& entry : m_items) {
			entry.item->setGraphicsElementStateFlags(_state);
		}
	}

	this->update();
}

void ot::GraphicsStackItem::notifyChildsAboutTransformChange(const QTransform& _newTransform) {
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

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

bool ot::GraphicsStackItem::adjustChildItems(void) {
	QSizeF masterSize(-1., -1.);
	for (auto itm : m_items) {
		if (itm.isMaster) {
			//OT_LOG_D("< MasterDetected { \"Item.Name\": \"" + itm.item->graphicsItemName() + "\", \"Width\": " + std::to_string(m_lastCalculatedSize.width()) + ", \"Height\": " + std::to_string(m_lastCalculatedSize.height()) + " }");
			masterSize = masterSize.expandedTo(itm.item->getQGraphicsItem()->boundingRect().size());
		}
	}

	if (masterSize.width() < 0. || masterSize.height() < 0.) {
		OT_LOG_WA("It appears that no master item was provided to this stack item");
		return false;
	}

	//OT_LOG_W("< Calculated for item \"" + this->graphicsItemName() + "\" w: " + std::to_string(masterSize.width()) + ", h: " + std::to_string(masterSize.height()));

	if (masterSize == m_lastCalculatedSize) return false;

	this->prepareGeometryChange();

	m_lastCalculatedSize = masterSize;

	for (const GraphicsStackItemEntry& itm : m_items) {
		if (itm.isSlave) {
			//OT_LOG_D("< SlaveDetected { \"Item.Name\": \"" + itm.item->graphicsItemName() + "\", \"Width\": " + std::to_string(m_lastCalculatedSize.width()) + ", \"Height\": " + std::to_string(m_lastCalculatedSize.height()) + " }");
			itm.item->setGraphicsItemRequestedSize(m_lastCalculatedSize);
		}
	}

	this->update();

	return true;
}
