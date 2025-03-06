//! @file TabToolBarGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/TabToolBar.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/TabToolBarPage.h"
#include "OTWidgets/TabToolBarGroup.h"
#include "OTWidgets/TabToolBarSubGroup.h"

// TabToolBar header
#include <TabToolbar/Group.h>

ot::TabToolBarGroup::TabToolBarGroup(TabToolBarPage* _parentPage, tt::Group* _group, const std::string& _name)
	: m_name(_name), m_group(_group), m_parentPage(_parentPage)
{
	OTAssertNullptr(m_group);
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

ot::TabToolBarSubGroup* ot::TabToolBarGroup::addSubGroup(const std::string& _subGroupName, bool _returnExisting) {
	TabToolBarSubGroup* newSubGroup = this->findSubGroup(_subGroupName);
	if (newSubGroup) {
		if (_returnExisting) {
			return newSubGroup;
		}
		else {
			OT_LOG_E("Subgroup \"" + _subGroupName + "\" already exists in group \"" + m_name + "\"");
			return nullptr;
		}
	}

	tt::SubGroup* group = m_group->AddSubGroup(tt::SubGroup::Align::Yes);

	newSubGroup = new TabToolBarSubGroup(this, group, _subGroupName);
	m_subGroups.push_back(newSubGroup);

	return newSubGroup;
}

void ot::TabToolBarGroup::forgetSubGroup(TabToolBarSubGroup* _subGroup) {
	auto it = std::find(m_subGroups.begin(), m_subGroups.end(), _subGroup);
	if (it != m_subGroups.end()) {
		m_subGroups.erase(it);
	}
}

ot::TabToolBarSubGroup* ot::TabToolBarGroup::findSubGroup(const std::string& _subGroupName) {
	for (TabToolBarSubGroup* subgroup : m_subGroups) {
		if (subgroup->getName() == _subGroupName) {
			return subgroup;
		}
	}
	return nullptr;
}

const ot::TabToolBarSubGroup* ot::TabToolBarGroup::findSubGroup(const std::string& _subGroupName) const {
	for (const TabToolBarSubGroup* subgroup : m_subGroups) {
		if (subgroup->getName() == _subGroupName) {
			return subgroup;
		}
	}
	return nullptr;
}

ot::ToolButton* ot::TabToolBarGroup::addToolButton(const QString& _iconPath, const QString& _text, QMenu* _menu) {
	ToolButton* btn = TabToolBar::createButton(_iconPath, _text, _menu);
	m_group->AddWidget(btn);
	return btn;
}

ot::ToolButton* ot::TabToolBarGroup::addToolButton(const QIcon& _icon, const QString& _text, QMenu* _menu) {
	ToolButton* btn = TabToolBar::createButton(_icon, _text, _menu);
	m_group->AddWidget(btn);
	return btn;
}
