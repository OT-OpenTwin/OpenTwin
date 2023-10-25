//! @file GraphicsLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/OTQtConverter.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OpenTwinCore/KeyMap.h"

ot::GraphicsLayoutItemWrapper::GraphicsLayoutItemWrapper(GraphicsLayoutItem* _owner) : m_owner(_owner) {
	OTAssertNullptr(m_owner);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsLayoutItemWrapper::~GraphicsLayoutItemWrapper() {}

void ot::GraphicsLayoutItemWrapper::prepareGraphicsItemGeometryChange(void) {
	this->prepareGeometryChange();
}

void ot::GraphicsLayoutItemWrapper::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	OTAssertNullptr(m_owner);
	m_owner->handleMousePressEvent(_event);
	QGraphicsWidget::mousePressEvent(_event);
}

void ot::GraphicsLayoutItemWrapper::mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) {
	OTAssertNullptr(m_owner);
	m_owner->handleMouseReleaseEvent(_event);
	QGraphicsWidget::mouseReleaseEvent(_event);
}

void ot::GraphicsLayoutItemWrapper::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	QGraphicsWidget::paint(_painter, _opt, _widget);
}

QVariant ot::GraphicsLayoutItemWrapper::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _value) {
	OTAssertNullptr(m_owner);
	m_owner->handleItemChange(_change, _value);
	return QGraphicsWidget::itemChange(_change, _value);
}

void ot::GraphicsLayoutItemWrapper::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsLayoutItemWrapper::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsLayoutItem::GraphicsLayoutItem() : m_layoutWrap(nullptr) {}

ot::GraphicsLayoutItem::~GraphicsLayoutItem() {}

bool ot::GraphicsLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	//if (m_layoutWrap) m_layoutWrap->resize(QSizeF(_cfg->size().width(), _cfg->size().height()));
	return ot::GraphicsItem::setupFromConfig(_cfg);
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
	m_layoutWrap->setParentGraphicsItem(this);
	m_layoutWrap->setLayout(_layout);
	// Refresh the parent item
	this->setParentGraphicsItem(nullptr);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsBoxLayoutItem::GraphicsBoxLayoutItem(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem) : QGraphicsLinearLayout(_orientation, _parentItem) 
{
	this->createLayoutWrapper(this);
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
			if (itm.second > 0) this->setStretchFactor(i->getQGraphicsLayoutItem(), itm.second);
			this->setAlignment(i->getQGraphicsLayoutItem(), ot::OTQtConverter::toQt(i->graphicsItemAlignment()));
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
	this->createLayoutWrapper(this);
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
				this->addItem(i->getQGraphicsLayoutItem(), x, y, OTQtConverter::toQt(i->graphicsItemAlignment()));
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