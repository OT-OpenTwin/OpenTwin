//! @file GraphcisVBoxLayoutItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/GraphicsVBoxLayoutItemCfg.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsVBoxLayoutItemCfg> vboxCfg(OT_FactoryKey_GraphicsVBoxLayoutItem);

ot::GraphicsVBoxLayoutItemCfg::GraphicsVBoxLayoutItemCfg() : GraphicsBoxLayoutItemCfg(ot::Vertical) {};

ot::GraphicsItemCfg* ot::GraphicsVBoxLayoutItemCfg::createCopy(void) const {
	ot::GraphicsVBoxLayoutItemCfg* copy = new GraphicsVBoxLayoutItemCfg;
	this->setupData(copy);

	return copy;
}
