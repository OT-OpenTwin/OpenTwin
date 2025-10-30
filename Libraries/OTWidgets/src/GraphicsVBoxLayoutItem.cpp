// @otlicense

// OpenTwin header
#include "OTGui/GraphicsVBoxLayoutItemCfg.h"
#include "OTWidgets/GraphicsVBoxLayoutItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

ot::GraphicsVBoxLayoutItem::GraphicsVBoxLayoutItem(QGraphicsLayoutItem* _parentItem)
	: GraphicsBoxLayoutItem(Qt::Vertical, new GraphicsVBoxLayoutItemCfg, _parentItem)
{

}

