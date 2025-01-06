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

ot::TabToolBarPage::TabToolBarPage(TabToolBar* _parentTabToolBar, tt::Page* _page, const std::string& _name) :
	m_name(_name), m_page(_page), m_parentTabToolBar(_parentTabToolBar)
{

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

QString ot::TabToolBarPage::getTitle(void) const {
	return m_page->objectName();
}

void ot::TabToolBarPage::forgetGroup(TabToolBarGroup* _group) {
	auto it = std::find(m_groups.begin(), m_groups.end(), _group);
	if (it != m_groups.end()) {
		m_groups.erase(it);
	}
}
