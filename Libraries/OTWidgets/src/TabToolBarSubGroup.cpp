//! @file TabToolBarSubGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TabToolBarGroup.h"
#include "OTWidgets/TabToolBarSubGroup.h"

// TabToolBar header
#include <TabToolbar/SubGroup.h>

ot::TabToolBarSubGroup::TabToolBarSubGroup(TabToolBarGroup* _parentGroup)
	: m_parentGroup(_parentGroup)
{
	m_subGroup = new tt::SubGroup(tt::SubGroup::Align::Yes);
}

ot::TabToolBarSubGroup::~TabToolBarSubGroup() {
	if (m_parentGroup) {
		m_parentGroup->forgetSubGroup(this);
	}

	delete m_subGroup;
	m_subGroup = nullptr;
}
