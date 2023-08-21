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
}

ot::GraphicsLayoutItemWrapper::~GraphicsLayoutItemWrapper() {}

void ot::GraphicsLayoutItemWrapper::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	this->handleItemClickEvent(_event, boundingRect());
	QGraphicsWidget::mousePressEvent(_event);
}

void ot::GraphicsLayoutItemWrapper::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	QGraphicsWidget::paint(_painter, _opt, _widget);
}

void ot::GraphicsLayoutItemWrapper::finalizeItem(GraphicsScene* _scene, GraphicsGroupItem* _group, bool _isRoot) {
	if (_group) _group->addToGroup(this);
	_scene->addItem(this);
}

void ot::GraphicsLayoutItemWrapper::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	this->paint(_painter, _opt, _widget);
}

void ot::GraphicsLayoutItemWrapper::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	this->setFlag(QGraphicsItem::ItemIsMovable, _flags & ot::GraphicsItem::ItemIsMoveable);
	this->setFlag(QGraphicsItem::ItemIsSelectable, _flags & ot::GraphicsItem::ItemIsMoveable);
}

QRectF ot::GraphicsLayoutItemWrapper::getGraphicsItemBoundingRect(void) const {
	return this->boundingRect();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsLayoutItem::GraphicsLayoutItem() : ot::GraphicsItem(true), m_layoutWrap(nullptr) {}

ot::GraphicsLayoutItem::~GraphicsLayoutItem() {}

bool ot::GraphicsLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	if (m_layoutWrap) m_layoutWrap->resize(QSizeF(_cfg->size().width(), _cfg->size().height()));
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

void ot::GraphicsLayoutItem::finalizeItem(GraphicsScene* _scene, GraphicsGroupItem* _group, bool _isRoot) {
	this->setGraphicsScene(_scene);

	std::list<QGraphicsLayoutItem*> lst;
	this->getAllItems(lst);

	for (auto itm : lst) {
		// Finalize child
		ot::GraphicsItem* gi = dynamic_cast<ot::GraphicsItem*>(itm);
		if (gi) gi->finalizeItem(_scene, _group, false);
		else {
			OT_LOG_EA("Failed to cast GrahicsLayoutItem child to GraphicsItem");
		}
	}

	if (_isRoot) {
		otAssert(m_layoutWrap == nullptr, "Should not be happening");

		QGraphicsLayout* lay = dynamic_cast<QGraphicsLayout *>(this);
		if (lay == nullptr) {
			OT_LOG_EA("OT::GraphicsLayoutItem cast to QGraphicsLayout failed");
		}
		else {
			// Add wrapped layout item
			m_layoutWrap = new GraphicsLayoutItemWrapper(this);
			m_layoutWrap->setParentGraphicsItem(this);
			m_layoutWrap->setLayout(lay);
			m_layoutWrap->finalizeItem(_scene, _group, false);
		}
	}
}

void ot::GraphicsLayoutItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	if (m_layoutWrap) {
		m_layoutWrap->graphicsItemFlagsChanged(_flags);
	}
}

void ot::GraphicsLayoutItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (m_layoutWrap) {
		m_layoutWrap->callPaint(_painter, _opt, _widget);
	}
	std::list<QGraphicsLayoutItem*> l;
	getAllItems(l);
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

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsBoxLayoutItem::GraphicsBoxLayoutItem(Qt::Orientation _orientation, QGraphicsLayoutItem* _parentItem) : QGraphicsLinearLayout(_orientation, _parentItem) {}

bool ot::GraphicsBoxLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsBoxLayoutItemCfg* cfg = dynamic_cast<ot::GraphicsBoxLayoutItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	for (auto itm : cfg->items()) {
		if (itm.first) {
			ot::GraphicsItem* i = ot::GraphicsFactory::itemFromConfig(itm.first);
			if (i == nullptr) {
				OT_LOG_EA("GraphicsFactory failed");
				return false;
			}
			QGraphicsLayoutItem* ii = dynamic_cast<QGraphicsLayoutItem*>(i);
			if (ii) {
				i->setParentGraphicsItem(this);
				this->addItem(ii);
			}
			else {
				OT_LOG_EA("GraphicsItem cast to QGraphicsLayoutItem failed");
				delete i;
			}
		}
		else {
			this->addStretch(itm.second);
		}
	}

	return GraphicsLayoutItem::setupFromConfig(_cfg);
}

QRectF ot::GraphicsBoxLayoutItem::getGraphicsItemBoundingRect(void) const {
	return this->contentsRect();
}

void ot::GraphicsBoxLayoutItem::getAllItems(std::list<QGraphicsLayoutItem *>& _items) const {
	for (int i = 0; i < this->count(); i++) {
		if (this->itemAt(i)) _items.push_back(this->itemAt(i));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsVBoxLayoutItem::GraphicsVBoxLayoutItem(QGraphicsLayoutItem* _parentItem) : GraphicsBoxLayoutItem(Qt::Vertical, _parentItem) {}

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

ot::GraphicsGridLayoutItem::GraphicsGridLayoutItem(QGraphicsLayoutItem* _parentItem) : QGraphicsGridLayout(_parentItem) {}

bool ot::GraphicsGridLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsGridLayoutItemCfg* cfg = dynamic_cast<ot::GraphicsGridLayoutItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

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
				QGraphicsLayoutItem* ii = dynamic_cast<QGraphicsLayoutItem*>(i);
				if (ii) {
					i->setParentGraphicsItem(this);
					this->addItem(ii, x, y);
				}
				else {
					OT_LOG_EA("GraphicsItem cast to QGraphicsLayoutItem failed");
				}
			}
			y++;
		}
		x++;
	}

	// Setup stretches
	x = 0;
	for (auto r : cfg->rowStretch()) {
		this->setRowStretchFactor(x++, r);
	}
	x = 0;
	for (auto r : cfg->columnStretch()) {
		this->setColumnStretchFactor(x++, r);
	}

	return GraphicsLayoutItem::setupFromConfig(_cfg);
}

QRectF ot::GraphicsGridLayoutItem::getGraphicsItemBoundingRect(void) const {
	return this->contentsRect();
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