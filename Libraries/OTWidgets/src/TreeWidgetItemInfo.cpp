//! @file TreeWidgetItemInfo.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/IconManager.h"
#include "OTWidgets/TreeWidgetItemInfo.h"

ot::TreeWidgetItemInfo::TreeWidgetItemInfo() : m_flags(ot::NoNavigationTreeItemFlags) {}

ot::TreeWidgetItemInfo::TreeWidgetItemInfo(const QString& _text, const QIcon& _icon, const NavigationTreeItemFlags& _flags) :
	m_text(_text), m_icon(_icon), m_flags(_flags)
{}

ot::TreeWidgetItemInfo::TreeWidgetItemInfo(const NavigationTreeItem& _config) :
	m_text(QString::fromStdString(_config.text())), m_flags(_config.flags())
{
	if (!_config.iconPath().empty()) {
		m_icon = IconManager::getIcon(QString::fromStdString(_config.iconPath()));
	}

	for (const NavigationTreeItem& child : _config.childItems()) {
		m_childs.push_back(TreeWidgetItemInfo(child));
	}
}

ot::TreeWidgetItemInfo::TreeWidgetItemInfo(const TreeWidgetItemInfo& _other) :
	m_text(_other.m_text), m_icon(_other.m_icon), m_flags(_other.m_flags), m_childs(_other.m_childs)
{}

ot::TreeWidgetItemInfo::~TreeWidgetItemInfo() {

}

ot::TreeWidgetItemInfo& ot::TreeWidgetItemInfo::operator = (const TreeWidgetItemInfo& _other) {
	m_text = _other.m_text;
	m_icon = _other.m_icon;
	m_flags = _other.m_flags;
	m_childs = _other.m_childs;
	return *this;
}

ot::TreeWidgetItemInfo& ot::TreeWidgetItemInfo::addChildItem(const QString& _text, const QIcon& _icon, const NavigationTreeItemFlags& _flags) {
	m_childs.push_back(TreeWidgetItemInfo(_text, _icon, _flags));
	return m_childs.back();
}

void ot::TreeWidgetItemInfo::addChildItem(const TreeWidgetItemInfo& _info) {
	m_childs.push_back(_info);
}

void ot::TreeWidgetItemInfo::clearChildItems(void) {
	m_childs.clear();
}
