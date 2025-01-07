//! @file TabToolBarPage.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
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

ot::TabToolBarGroup* ot::TabToolBarPage::addGroup(const std::string& _groupName, bool _returnExisting) {
	TabToolBarGroup* newGroup = this->findGroup(_groupName);
	if (newGroup) {
		if (_returnExisting) {
			return newGroup;
		}
		else {
			OT_LOG_E("Group \"" + _groupName + "\" already exists in page \"" + m_name + "\"");
			return nullptr;
		}
	}

	tt::Group* group = m_page->AddGroup(QString::fromStdString(_groupName));

	newGroup = new TabToolBarGroup(this, group, _groupName);
	m_groups.push_back(newGroup);

	return newGroup;
}

ot::TabToolBarSubGroup* ot::TabToolBarPage::addSubGroup(const std::string& _groupName, const std::string& _subGroupName, bool _returnExisting) {
	TabToolBarGroup* group = this->findGroup(_groupName);
	if (!group) {
		OT_LOG_E("Group \"" + _groupName + "\" not found in page \"" + m_name + "\"");
		return nullptr;
	}
	else {
		return group->addSubGroup(_subGroupName, _returnExisting);
	}
}

void ot::TabToolBarPage::forgetGroup(TabToolBarGroup* _group) {
	auto it = std::find(m_groups.begin(), m_groups.end(), _group);
	if (it != m_groups.end()) {
		m_groups.erase(it);
	}
}

ot::TabToolBarGroup* ot::TabToolBarPage::findGroup(const std::string& _groupName) {
	for (TabToolBarGroup* group : m_groups) {
		if (group->getName() == _groupName) {
			return group;
		}
	}
	return nullptr;
}

const ot::TabToolBarGroup* ot::TabToolBarPage::findGroup(const std::string& _groupName) const {
	for (const TabToolBarGroup* group : m_groups) {
		if (group->getName() == _groupName) {
			return group;
		}
	}
	return nullptr;
}

ot::TabToolBarSubGroup* ot::TabToolBarPage::findSubGroup(const std::string& _groupName, const std::string& _subGroupName) {
	for (TabToolBarGroup* group : m_groups) {
		if (group->getName() == _groupName) {
			return group->findSubGroup(_subGroupName);
		}
	}
	return nullptr;
}

const ot::TabToolBarSubGroup* ot::TabToolBarPage::findSubGroup(const std::string& _groupName, const std::string& _subGroupName) const {
	for (const TabToolBarGroup* group : m_groups) {
		if (group->getName() == _groupName) {
			return group->findSubGroup(_subGroupName);
		}
	}
	return nullptr;
}
