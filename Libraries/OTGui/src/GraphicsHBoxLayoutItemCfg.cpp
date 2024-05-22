//! @file GraphcisHBoxLayoutItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsHBoxLayoutItemCfg> ellipseItemRegistrar(OT_FactoryKey_GraphicsHBoxLayoutItem);

ot::GraphicsHBoxLayoutItemCfg::GraphicsHBoxLayoutItemCfg() : GraphicsBoxLayoutItemCfg(ot::Horizontal) {};

ot::GraphicsItemCfg* ot::GraphicsHBoxLayoutItemCfg::createCopy(void) const {
	ot::GraphicsHBoxLayoutItemCfg* copy = new GraphicsHBoxLayoutItemCfg;
	this->setupData(copy);

	return copy;
}
