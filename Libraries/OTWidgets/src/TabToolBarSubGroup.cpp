//! @file TabToolBarSubGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/TabToolBar.h"
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

void ot::TabToolBarSubGroup::addToolButton(const QString& _iconPath, const QString& _text, QMenu* _menu) {
	m_subGroup->AddWidget(TabToolBar::createButton(_iconPath, _text, _menu));
}

void ot::TabToolBarSubGroup::addToolButton(const QIcon& _icon, const QString& _text, QMenu* _menu) {
	m_subGroup->AddWidget(TabToolBar::createButton(_icon, _text, _menu));
}
