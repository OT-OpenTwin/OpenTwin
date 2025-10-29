// @otlicense

//! @file MenuSeparatorCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/MenuSeparatorCfg.h"
#include "OTGui/MenuEntryCfgFactory.h"

static ot::MenuEntryCfgFactoryRegistrar<ot::MenuSeparatorCfg> separatorRegistarar(ot::MenuEntryCfg::toString(ot::MenuEntryCfg::Separator));

ot::MenuSeparatorCfg::MenuSeparatorCfg() {

}

ot::MenuSeparatorCfg::MenuSeparatorCfg(const MenuSeparatorCfg& _other)
	: MenuEntryCfg(_other)
{

}

ot::MenuSeparatorCfg::~MenuSeparatorCfg() {

}

ot::MenuEntryCfg* ot::MenuSeparatorCfg::createCopy(void) const {
	return new MenuSeparatorCfg(*this);
}
