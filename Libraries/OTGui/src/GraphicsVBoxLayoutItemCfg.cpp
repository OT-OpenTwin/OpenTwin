//! @file GraphcisVBoxLayoutItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsVBoxLayoutItemCfg.h"

static ot::SimpleFactoryRegistrar<ot::GraphicsVBoxLayoutItemCfg> vboxCfg(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItemCfg);

ot::GraphicsVBoxLayoutItemCfg::GraphicsVBoxLayoutItemCfg() : GraphicsBoxLayoutItemCfg(ot::Vertical) {};
