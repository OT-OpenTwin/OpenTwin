//! @file TabToolBarGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TabToolBarPage.h"
#include "OTWidgets/TabToolBarGroup.h"
#include "OTWidgets/TabToolBarSubGroup.h"

// TabToolBar header
#include <TabToolbar/Group.h>

ot::TabToolBarGroup::TabToolBarGroup(TabToolBarPage* _parentPage, const QString& _name)
	: m_parentPage(_parentPage) {
	m_group = new tt::Group(_name);
}

ot::TabToolBarGroup::~TabToolBarGroup() {
	if (m_parentPage) {
		m_parentPage->forgetGroup(this);
	}
	for (TabToolBarSubGroup* sub : m_subGroups) {
		sub->setParentTabToolBarGroup(nullptr);
	}

	delete m_group;
	m_group = nullptr;
}

void ot::TabToolBarGroup::forgetSubGroup(TabToolBarSubGroup* _subGroup) {
	auto it = std::find(m_subGroups.begin(), m_subGroups.end(), _subGroup);
	if (it != m_subGroups.end()) {
		m_subGroups.erase(it);
	}
}
