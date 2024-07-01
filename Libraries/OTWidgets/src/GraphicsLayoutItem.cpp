//! @file GraphicsLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsLayoutItemWrapper.h"

ot::GraphicsLayoutItem::GraphicsLayoutItem(GraphicsItemCfg* _configuration)
	: ot::GraphicsItem(_configuration, GraphicsItem::ForwardSizeState), m_layoutWrap(nullptr)
{}

ot::GraphicsLayoutItem::~GraphicsLayoutItem() {
	if (m_layoutWrap) {
		m_layoutWrap->unsetGraphicsLayoutItem();
		m_layoutWrap->setStateFlag(GraphicsItem::ToBeDeletedState, true);
	}
}

bool ot::GraphicsLayoutItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->setAcceptHoverEvents(true);
	m_layoutWrap->setBlockConfigurationNotifications(true);
	bool ret = ot::GraphicsItem::setupFromConfig(_cfg);
	m_layoutWrap->setupFromConfig(this->getConfiguration());
	m_layoutWrap->setBlockConfigurationNotifications(false);
	return ret;
}

void ot::GraphicsLayoutItem::setGraphicsItemName(const std::string& _name) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->setGraphicsItemName(_name);
	this->setBlockConfigurationNotifications(true);
	ot::GraphicsItem::setGraphicsItemName(_name + "_AL");
	this->setBlockConfigurationNotifications(false);
}

void ot::GraphicsLayoutItem::removeAllConnections(void) {
	ot::GraphicsItem::removeAllConnections();

	std::list<QGraphicsLayoutItem*> items;
	this->getAllItems(items);

	for (auto i : items) {
		ot::GraphicsItem* item = dynamic_cast<ot::GraphicsItem*>(i);
		if (item) {
			item->removeAllConnections();
		}
		else {
			OT_LOG_EA("GraphicsItem cast failed");
		}
	}

	if (m_layoutWrap) {
		m_layoutWrap->unsetGraphicsLayoutItem();
		m_layoutWrap->removeAllConnections();
		m_layoutWrap->setGraphicsLayoutItem(this);
	}
}

void ot::GraphicsLayoutItem::prepareGraphicsItemGeometryChange(void) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->prepareGraphicsItemGeometryChange();
}

void ot::GraphicsLayoutItem::setParentGraphicsItem(GraphicsItem* _itm) {
	if (m_layoutWrap) {
		m_layoutWrap->setParentGraphicsItem(_itm);
	}
	ot::GraphicsItem::setParentGraphicsItem(m_layoutWrap);
}

void ot::GraphicsLayoutItem::setGraphicsScene(GraphicsScene* _scene) {
	OTAssertNullptr(m_layoutWrap);
	GraphicsItem::setGraphicsScene(_scene);
	m_layoutWrap->setGraphicsScene(_scene);
}

void ot::GraphicsLayoutItem::graphicsItemFlagsChanged(const GraphicsItemCfg::GraphicsItemFlags& _flags) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->setGraphicsItemFlags(_flags);
}

void ot::GraphicsLayoutItem::graphicsItemStateChanged(const GraphicsItem::GraphicsItemStateFlags& _state) {
	if (m_layoutWrap) {
		m_layoutWrap->setBlockStateNotifications(true);
		m_layoutWrap->setGraphicsItemState(_state);
		m_layoutWrap->setBlockStateNotifications(false);
	}

	if (this->getGraphicsItemFlags() & GraphicsItemCfg::GraphicsItemFlag::ItemForwardsState || this->getGraphicsItemState() & GraphicsItem::ToBeDeletedState) {
		std::list<QGraphicsLayoutItem*> lst;
		this->getAllItems(lst);
		for (auto i : lst) {
			ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(i);
			if (itm) {
				itm->setGraphicsItemState(_state);
			}
		}
	}

	if (!(this->getGraphicsItemState() & GraphicsItem::ToBeDeletedState)) {
		m_layoutWrap->update();
	}
}

void ot::GraphicsLayoutItem::graphicsItemConfigurationChanged(const GraphicsItemCfg* _config) {
	OTAssertNullptr(_config);
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->setConfiguration(_config->createCopy());
	this->setGraphicsItemName(_config->getName()); // Wrap has a different name, therefore we update our name

	ot::GraphicsItem::graphicsItemConfigurationChanged(_config);
}

void ot::GraphicsLayoutItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->callPaint(_painter, _opt, _widget);
}

QGraphicsLayoutItem* ot::GraphicsLayoutItem::getQGraphicsLayoutItem(void) { 
	return m_layoutWrap->getQGraphicsLayoutItem(); 
};

QGraphicsItem* ot::GraphicsLayoutItem::getQGraphicsItem(void) { return (m_layoutWrap ? m_layoutWrap->getQGraphicsItem() : nullptr); };

const QGraphicsItem* ot::GraphicsLayoutItem::getQGraphicsItem(void) const { return (m_layoutWrap ? m_layoutWrap->getQGraphicsItem() : nullptr); };

ot::GraphicsItem* ot::GraphicsLayoutItem::findItem(const std::string& _itemName) {
	if (_itemName == this->getGraphicsItemName()) return this;
	std::list<QGraphicsLayoutItem*> lst;
	this->getAllItems(lst);
	for (auto i : lst) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(i);
		if (itm) {
			auto r = itm->findItem(_itemName);
			if (r) return r;
		}
	}
	return nullptr;
}

void ot::GraphicsLayoutItem::finalizeGraphicsItem(void) {
	std::list<QGraphicsLayoutItem*> items;
	this->getAllItems(items);

	for (auto i : items) {
		ot::GraphicsItem* item = dynamic_cast<ot::GraphicsItem*>(i);
		if (item) {
			item->finalizeGraphicsItem();
		}
		else {
			OT_LOG_EA("GraphicsItem cast failed");
		}
	}
}

void ot::GraphicsLayoutItem::setGraphicsItemRequestedSize(const QSizeF& _size) {
	OTAssertNullptr(m_layoutWrap);
	ot::GraphicsItem::setGraphicsItemRequestedSize(_size);
	m_layoutWrap->setGraphicsItemRequestedSize(_size);
}

void ot::GraphicsLayoutItem::unsetLayoutWrapper(void) {
	m_layoutWrap = nullptr;
	this->setParentGraphicsItem(nullptr);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::GraphicsLayoutItem::notifyChildsAboutTransformChange(const QTransform& _newTransform) {
	std::list<QGraphicsLayoutItem*> itemsList;
	this->getAllItems(itemsList);
	for (auto i : itemsList) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem*>(i);
		if (itm) {
			itm->parentItemTransformChanged(_newTransform);
		}
		else {
			OT_LOG_EA("Item cast failed");
		}
	}
}

void ot::GraphicsLayoutItem::createLayoutWrapper(QGraphicsLayout* _layout) {
	OTAssert(m_layoutWrap == nullptr, "Layout wrapper already created");
	m_layoutWrap = new GraphicsLayoutItemWrapper(this, _layout);	

	// Refresh the parent item
	this->setParentGraphicsItem(nullptr);
}
