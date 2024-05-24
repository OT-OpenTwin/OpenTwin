//! @file GraphicsHBoxLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GraphicsHBoxLayoutItem.h"

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsHBoxLayoutItem> hblayItemRegistrar(OT_FactoryKey_GraphicsHBoxLayoutItem);

ot::GraphicsHBoxLayoutItem::GraphicsHBoxLayoutItem(QGraphicsLayoutItem* _parentItem)
	: GraphicsBoxLayoutItem(Qt::Horizontal, new GraphicsHBoxLayoutItemCfg, _parentItem) 
{}

bool ot::GraphicsHBoxLayoutItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	return ot::GraphicsBoxLayoutItem::setupFromConfig(_cfg);
}

