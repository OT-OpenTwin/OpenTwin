//! @file GraphicsVBoxLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsVBoxLayoutItemCfg.h"
#include "OTWidgets/GraphicsVBoxLayoutItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsVBoxLayoutItem> vblayItemRegistrar(OT_FactoryKey_GraphicsVBoxLayoutItem);

ot::GraphicsVBoxLayoutItem::GraphicsVBoxLayoutItem(QGraphicsLayoutItem* _parentItem)
	: GraphicsBoxLayoutItem(Qt::Vertical, _parentItem)
{

}

bool ot::GraphicsVBoxLayoutItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	return ot::GraphicsBoxLayoutItem::setupFromConfig(_cfg);
}

