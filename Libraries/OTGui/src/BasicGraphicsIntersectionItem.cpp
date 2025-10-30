// @otlicense

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/BasicGraphicsIntersectionItem.h"

ot::BasicGraphicsIntersectionItem::BasicGraphicsIntersectionItem() 
	: GraphicsEllipseItemCfg(5., 5., nullptr)
{
	this->setBackgroundPainer(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder));
	this->setGraphicsItemFlags(GraphicsItemFlag::ItemHandlesState | GraphicsItemFlag::ItemIsConnectable | GraphicsItemFlag::ItemIsMoveable | GraphicsItemFlag::ItemIsSelectable | GraphicsItemFlag::ItemSnapsToGridCenter);
}