//! @file GraphicsLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OpenTwinCore/KeyMap.h"

ot::GraphicsLayoutItemWrapper::GraphicsLayoutItemWrapper(GraphicsLayoutItem* _owner) : m_owner(_owner) {
	OTAssertNullptr(m_owner);
	m_group = new ot::GraphicsGroupItem;
	m_group->addToGroup(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
	this->setParentGraphicsItem(m_group);
}

ot::GraphicsLayoutItemWrapper::~GraphicsLayoutItemWrapper() {}

void ot::GraphicsLayoutItemWrapper::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	this->handleItemClickEvent(_event, boundingRect());
	QGraphicsWidget::mousePressEvent(_event);
}

void ot::GraphicsLayoutItemWrapper::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	QGraphicsWidget::paint(_painter, _opt, _widget);
}

QVariant ot::GraphicsLayoutItemWrapper::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		m_owner->handleItemMoved();
		m_owner->raiseEvent(ot::GraphicsItem::Resized);
	}
	return QGraphicsWidget::itemChange(_change, _value);
}

void ot::GraphicsLayoutItemWrapper::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsLayoutItemWrapper::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	OTAssertNullptr(m_group);
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
	m_group->setGraphicsItemFlags(_flags);
}

QRectF ot::GraphicsLayoutItemWrapper::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

QPointF ot::GraphicsLayoutItemWrapper::getGraphicsItemScenePos(void) const {
	return this->scenePos();
}

void ot::GraphicsLayoutItemWrapper::setParentGraphicsItem(GraphicsItem* _itm) {
	OTAssertNullptr(m_group);
	m_group->setParentGraphicsItem(_itm);
	ot::GraphicsItem::setParentGraphicsItem(m_group);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsLayoutItem::GraphicsLayoutItem() : ot::GraphicsItem(true), m_layoutWrap(nullptr) {}

ot::GraphicsLayoutItem::~GraphicsLayoutItem() {}

bool ot::GraphicsLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	//if (m_layoutWrap) m_layoutWrap->resize(QSizeF(_cfg->size().width(), _cfg->size().height()));
	return ot::GraphicsItem::setupFromConfig(_cfg);
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
	if (m_layoutWrap) {
		m_layoutWrap->callPaint(_painter, _opt, _widget);
	}
	std::list<QGraphicsLayoutItem*> l;
	this->getAllItems(l);
	for (auto i : l) {
		ot::GraphicsItem* itm = dynamic_cast<ot::GraphicsItem *>(i);
		if (itm) {
			itm->callPaint(_painter, _opt, _widget);
		}
		else {
			OT_LOG_EA("Item cast failed");
		}
	}
}

QRectF ot::GraphicsLayoutItem::getGraphicsItemBoundingRect(void) const {
	OTAssertNullptr(m_layoutWrap);
	return m_layoutWrap->boundingRect();
}

QPointF ot::GraphicsLayoutItem::getGraphicsItemScenePos(void) const {
	OTAssertNullptr(m_layoutWrap);
	return m_layoutWrap->scenePos();
}

void ot::GraphicsLayoutItem::createLayoutWrapperAndGroup(QGraphicsLayout* _layout) {
	otAssert(m_layoutWrap == nullptr, "Layout wrapper already created");
	m_layoutWrap = new GraphicsLayoutItemWrapper(this);
	m_layoutWrap->setParentGraphicsItem(this);
	m_layoutWrap->setLayout(_layout);
	// Refresh the parent item
	this->setParentGraphicsItem(nullptr);
}

void ot::GraphicsLayoutItem::addChildToGroup(ot::GraphicsItem* _item) {
	OTAssertNullptr(m_layoutWrap);
	m_layoutWrap->getGroupItem()->addToGroup(_item->getQGraphicsItem());
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsBoxLayoutItem::GraphicsBoxLayoutItem(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem) : QGraphicsLinearLayout(_orientation, _parentItem) 
{
	this->createLayoutWrapperAndGroup(this);
}

bool ot::GraphicsBoxLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsBoxLayoutItemCfg* cfg = dynamic_cast<ot::GraphicsBoxLayoutItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->setContentsMargins(_cfg->margins().left(), _cfg->margins().top(), _cfg->margins().right(), _cfg->margins().bottom());

	for (auto itm : cfg->items()) {
		if (itm.first) {
			ot::GraphicsItem* i = ot::GraphicsFactory::itemFromConfig(itm.first);
			if (i == nullptr) {
				OT_LOG_EA("GraphicsFactory failed");
				return false;
			}
			i->setParentGraphicsItem(this);
			OTAssertNullptr(i->getQGraphicsLayoutItem());
			this->addItem(i->getQGraphicsLayoutItem());
			this->addChildToGroup(i);
			if (itm.second > 0) this->setStretchFactor(i->getQGraphicsLayoutItem(), itm.second);
		}
		else {
			this->addStretch(itm.second);
		}
	}

	return GraphicsLayoutItem::setupFromConfig(_cfg);
}

void ot::GraphicsBoxLayoutItem::getAllItems(std::list<QGraphicsLayoutItem *>& _items) const {
	for (int i = 0; i < this->count(); i++) {
		if (this->itemAt(i)) _items.push_back(this->itemAt(i));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsVBoxLayoutItem::GraphicsVBoxLayoutItem(QGraphicsLayoutItem* _parentItem) 
	: GraphicsBoxLayoutItem(Qt::Vertical, _parentItem) 
{

}

bool ot::GraphicsVBoxLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	return ot::GraphicsBoxLayoutItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsHBoxLayoutItem::GraphicsHBoxLayoutItem(QGraphicsLayoutItem* _parentItem) : GraphicsBoxLayoutItem(Qt::Horizontal, _parentItem) {}

bool ot::GraphicsHBoxLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	return ot::GraphicsBoxLayoutItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsGridLayoutItem::GraphicsGridLayoutItem(QGraphicsLayoutItem* _parentItem) : QGraphicsGridLayout(_parentItem) 
{
	this->createLayoutWrapperAndGroup(this);
}

bool ot::GraphicsGridLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsGridLayoutItemCfg* cfg = dynamic_cast<ot::GraphicsGridLayoutItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->setContentsMargins(_cfg->margins().left(), _cfg->margins().top(), _cfg->margins().right(), _cfg->margins().bottom());

	// Create items
	int x = 0;
	for (auto r : cfg->items()) {
		int y = 0;
		for (auto c : r) {
			if (c) {
				ot::GraphicsItem* i = ot::GraphicsFactory::itemFromConfig(c);
				if (i == nullptr) {
					OT_LOG_EA("GraphicsFactory failed");
					return false;
				}
				i->setParentGraphicsItem(this);
				OTAssertNullptr(i->getQGraphicsLayoutItem());
				this->addItem(i->getQGraphicsLayoutItem(), x, y);
				this->addChildToGroup(i);
			}
			y++;
		}
		x++;
	}

	// Setup stretches
	for (size_t r = 0; r < cfg->rowStretch().size(); r++) {
		this->setRowStretchFactor(r, cfg->rowStretch()[r]);
	}
	for (size_t c = 0; c < cfg->columnStretch().size(); c++) {
		this->setColumnStretchFactor(c, cfg->columnStretch()[c]);
	}
	
	return GraphicsLayoutItem::setupFromConfig(_cfg);
}

void ot::GraphicsGridLayoutItem::getAllItems(std::list<QGraphicsLayoutItem*>& _items) const {
	for (int r = 0; r < this->rowCount(); r++) {
		for (int c = 0; c < this->columnCount(); c++) {
			if (this->itemAt(r, c)) _items.push_back(this->itemAt(r, c));
		}
	}
}

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsVBoxLayoutItem> vBoxItem(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsHBoxLayoutItem> hBoxItem(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsGridLayoutItem> gridItem(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem);

// Register at global key map (config -> item)
static ot::GlobalKeyMapRegistrar vBoxItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem);
static ot::GlobalKeyMapRegistrar hBoxItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem);
static ot::GlobalKeyMapRegistrar gridItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem);