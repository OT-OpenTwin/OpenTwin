//! @file TabToolBarSubGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TabToolBarGroup.h"
#include "OTWidgets/TabToolBarSubGroup.h"

// TabToolBar header
#include <TabToolbar/SubGroup.h>

ot::TabToolBarSubGroup::TabToolBarSubGroup(TabToolBarGroup* _parentGroup, tt::SubGroup* _subGroup, const std::string& _name)
	: m_name(_name), m_subGroup(_subGroup), m_parentGroup(_parentGroup)
{

}

ot::TabToolBarSubGroup::~TabToolBarSubGroup() {
	if (m_parentGroup) {
		m_parentGroup->forgetSubGroup(this);
	}

	delete m_subGroup;
	m_subGroup = nullptr;
}

void ot::TabToolBarSubGroup::addWidget(QWidget* _widget) {
	m_subGroup->AddWidget(_widget);
}
