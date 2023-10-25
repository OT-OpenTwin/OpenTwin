//! @file GraphicsStackItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/KeyMap.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTWidgets/GraphicsStackItem.h"

static ot::SimpleFactoryRegistrar<ot::GraphicsStackItem> stackItem(OT_SimpleFactoryJsonKeyValue_GraphicsStackItem);
static ot::GlobalKeyMapRegistrar stackItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsStackItem);

ot::GraphicsStackItem::GraphicsStackItem() : m_isFirstPaint(true) {

}

ot::GraphicsStackItem::~GraphicsStackItem() {
	this->memClear();
}

bool ot::GraphicsStackItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsStackItemCfg* cfg = dynamic_cast<ot::GraphicsStackItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->memClear();

	for (auto itm : cfg->items()) {
		OTAssertNullptr(itm.item);

		ot::GraphicsItem* i = nullptr;
		try {
			i = ot::GraphicsFactory::itemFromConfig(itm.item);
			if (i) {
				GraphicsStackItemEntry e;
				e.isMaster = itm.isMaster;
				e.isSlave = itm.isSlave;
				e.item = i;
				i->setParentGraphicsItem(this);
				if (e.isMaster) {
					// If the item is a master item, install an event filter for resizing the child items
					i->addGraphicsItemEventHandler(this);
				}
				m_items.push_back(e);

				this->addToGroup(e.item->getQGraphicsItem());
			}
			else {
				OT_LOG_EA("Failed to created graphics item from factory");
			}
		}
		catch (const std::exception& _e) {
			OT_LOG_EAS("Failed to create child item: " + std::string(_e.what()));
			if (i) delete i;
			throw _e;
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
			if (i) delete i;
			throw std::exception("[FATAL] Unknown error");
		}
	}

	return ot::GraphicsGroupItem::setupFromConfig(_cfg);
}

void ot::GraphicsStackItem::callPaint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	ot::GraphicsGroupItem::callPaint(_painter, _opt, _widget);
	//for (auto itm : m_items) itm.item->callPaint(_painter, _opt, _widget);
}

void ot::GraphicsStackItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget) {
	if (m_isFirstPaint) {
		this->adjustChildItems();
		m_isFirstPaint = false;
	}
	ot::GraphicsGroupItem::paint(_painter, _opt, _widget);
}

void ot::GraphicsStackItem::graphicsItemFlagsChanged(ot::GraphicsItem::GraphicsItemFlag _flags) {
	ot::GraphicsGroupItem::graphicsItemFlagsChanged(_flags);
}

void ot::GraphicsStackItem::graphicsItemEventHandler(ot::GraphicsItem* _sender, GraphicsItemEvent _event) {
	if (_event == ot::GraphicsItem::ItemResized) {
		this->adjustChildItems();
	}
}

void ot::GraphicsStackItem::memClear(void) {
	for (auto itm : m_items) delete itm.item;
	m_items.clear();
}

void ot::GraphicsStackItem::adjustChildItems(void) {
	QSizeF masterSize(-1., -1.);
	for (auto itm : m_items) {
		if (itm.isMaster) {
			//OT_LOG_D("< Handling ItemResized: MasterDetected { \"Root.UID\": \"" + itm.item->getRootItem()->graphicsItemUid() + "\", \"Item.Name\": \"" + itm.item->graphicsItemName() + "\" }");
			masterSize = masterSize.expandedTo(itm.item->getQGraphicsItem()->boundingRect().size());
		}
	}

	if (masterSize.width() < 0. || masterSize.height() < 0.) {
		OT_LOG_WA("It appears that no master item was provided to this stack item");
		return;
	}

	for (GraphicsStackItemEntry itm : m_items) {
		if (itm.isSlave) {
			//OT_LOG_D("< Handling ItemResized: SlaveDetected { \"Root.UID\": \"" + itm.item->getRootItem()->graphicsItemUid() + "\", \"Item.Name\": \"" + itm.item->graphicsItemName() + "\" }");
			itm.item->setGraphicsItemRequestedSize(masterSize);
		}
	}
}
