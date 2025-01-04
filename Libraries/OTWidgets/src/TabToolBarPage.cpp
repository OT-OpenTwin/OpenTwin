//! @file TabToolBarPage.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TabToolBar.h"
#include "OTWidgets/TabToolBarPage.h"
#include "OTWidgets/TabToolBarGroup.h"

// TabToolBar header
#include <TabToolbar/Page.h>

ot::TabToolBarPage::TabToolBarPage(TabToolBar* _parentTabToolBar, int _index, const QString& _name)
	: m_parentTabToolBar(_parentTabToolBar)
{
	m_page = new tt::Page(_index, _name);
}

ot::TabToolBarPage::~TabToolBarPage() {
	if (m_parentTabToolBar) {
		m_parentTabToolBar->forgetPage(this);
	}
	for (TabToolBarGroup* group : m_groups) {
		group->setParentTabToolBarPage(nullptr);
	}

	delete m_page;
	m_page = nullptr;
}

void ot::TabToolBarPage::forgetGroup(TabToolBarGroup* _group) {
	auto it = std::find(m_groups.begin(), m_groups.end(), _group);
	if (it != m_groups.end()) {
		m_groups.erase(it);
	}
}
