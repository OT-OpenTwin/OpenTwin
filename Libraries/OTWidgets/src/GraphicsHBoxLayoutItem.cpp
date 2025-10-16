//! @file GraphicsHBoxLayoutItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTWidgets/GraphicsHBoxLayoutItem.h"

ot::GraphicsHBoxLayoutItem::GraphicsHBoxLayoutItem(QGraphicsLayoutItem* _parentItem)
	: GraphicsBoxLayoutItem(Qt::Horizontal, new GraphicsHBoxLayoutItemCfg, _parentItem) 
{}
