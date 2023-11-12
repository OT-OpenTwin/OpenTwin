//! @file GraphicsLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsLayoutItemWrapper.h"
#include "OTGui/GraphicsLayoutItemCfg.h"

ot::GraphicsLayoutItem::GraphicsLayoutItem()
	: ot::GraphicsItem(true), m_layoutWrap(nullptr)
{}

ot::GraphicsLayoutItem::~GraphicsLayoutItem() {

}

bool ot::GraphicsLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	//if (m_layoutWrap) m_layoutWrap->resize(QSizeF(_cfg->size().width(), _cfg->size().height()));
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsLayoutItem::setGraphicsItemName(const std::string& _name) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->setGraphicsItemName(_name);
	ot::GraphicsItem::setGraphicsItemName(_name + "_AL");
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
}

void ot::GraphicsLayoutItem::prepareGraphicsItemGeometryChange(void) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->prepareGraphicsItemGeometryChange();
}

void ot::GraphicsLayoutItem::setParentGraphicsItem(GraphicsItem* _itm) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->setParentGraphicsItem(_itm);
	ot::GraphicsItem::setParentGraphicsItem(m_layoutWrap);
}

void ot::GraphicsLayoutItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->setGraphicsItemFlags(_flags);
}

void ot::GraphicsLayoutItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->callPaint(_painter, _opt, _widget);
}

QGraphicsLayoutItem* ot::GraphicsLayoutItem::getQGraphicsLayoutItem(void) { return m_layoutWrap->getQGraphicsLayoutItem(); };

QGraphicsItem* ot::GraphicsLayoutItem::getQGraphicsItem(void) { return m_layoutWrap->getQGraphicsItem(); };

ot::GraphicsItem* ot::GraphicsLayoutItem::findItem(const std::string& _itemName) {
	if (_itemName == this->graphicsItemName()) return this;
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

void ot::GraphicsLayoutItem::createLayoutWrapper(QGraphicsLayout* _layout) {
	otAssert(m_layoutWrap == nullptr, "Layout wrapper already created");
	m_layoutWrap = new GraphicsLayoutItemWrapper(this);
	m_layoutWrap->setLayout(_layout);

	// Refresh the parent item
	this->setParentGraphicsItem(nullptr);
}
