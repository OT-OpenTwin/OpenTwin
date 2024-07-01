//! @file GraphcisHBoxLayoutItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsHBoxLayoutItemCfg> ellipseItemRegistrar(OT_FactoryKey_GraphicsHBoxLayoutItem);

ot::GraphicsHBoxLayoutItemCfg::GraphicsHBoxLayoutItemCfg() : GraphicsBoxLayoutItemCfg(ot::Horizontal) {};

ot::GraphicsHBoxLayoutItemCfg::GraphicsHBoxLayoutItemCfg(const GraphicsHBoxLayoutItemCfg& _other) : GraphicsBoxLayoutItemCfg(_other) {}

ot::GraphicsItemCfg* ot::GraphicsHBoxLayoutItemCfg::createCopy(void) const {
	return new GraphicsHBoxLayoutItemCfg(*this);
}
