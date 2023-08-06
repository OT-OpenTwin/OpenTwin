//! @file GraphicsLayout.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsFactory.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OpenTwinCore/KeyMap.h"

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
				this->addItem(ii);
			}
			else {
				OT_LOG_EA("GraphicsItem cast to QGraphicsLayoutItem failed");
			}
		}
		else {
			this->addStretch(itm.second);
		}
	}

	return true;
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


	return true;
}

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsVBoxLayoutItem> vBoxItem(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsHBoxLayoutItem> hBoxItem(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem);
static ot::SimpleFactoryRegistrar<ot::GraphicsGridLayoutItem> gridItem(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem);

// Register at global key map (config -> item)
static ot::GlobalKeyMapRegistrar vBoxItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItem);
static ot::GlobalKeyMapRegistrar hBoxItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItem);
static ot::GlobalKeyMapRegistrar gridItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItem);