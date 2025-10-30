// @otlicense

// OpenTwin header
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTWidgets/GraphicsHBoxLayoutItem.h"

ot::GraphicsHBoxLayoutItem::GraphicsHBoxLayoutItem(QGraphicsLayoutItem* _parentItem)
	: GraphicsBoxLayoutItem(Qt::Horizontal, new GraphicsHBoxLayoutItemCfg, _parentItem) 
{}
