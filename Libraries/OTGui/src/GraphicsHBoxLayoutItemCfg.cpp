//! @file GraphcisHBoxLayoutItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"

static ot::SimpleFactoryRegistrar<ot::GraphicsHBoxLayoutItemCfg> hboxCfg(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItemCfg);

ot::GraphicsHBoxLayoutItemCfg::GraphicsHBoxLayoutItemCfg() : GraphicsBoxLayoutItemCfg(ot::Horizontal) {};