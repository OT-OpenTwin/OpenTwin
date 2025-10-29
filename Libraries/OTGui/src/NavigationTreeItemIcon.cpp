// @otlicense

//! @file NavigationTreeItemIcon.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/NavigationTreeItemIcon.h"

ot::NavigationTreeItemIcon::NavigationTreeItemIcon() {}

ot::NavigationTreeItemIcon::NavigationTreeItemIcon(const std::string& _visibleIcon, const std::string& _hiddenIcon)
	: m_visibleIcon(_visibleIcon), m_hiddenIcon(_hiddenIcon)
{}

ot::NavigationTreeItemIcon::NavigationTreeItemIcon(const NavigationTreeItemIcon& _other) {
	*this = _other;
}

ot::NavigationTreeItemIcon::~NavigationTreeItemIcon() {}

ot::NavigationTreeItemIcon& ot::NavigationTreeItemIcon::operator = (const NavigationTreeItemIcon& _other) {
	if (this == &_other) return *this;

	m_visibleIcon = _other.m_visibleIcon;
	m_hiddenIcon = _other.m_hiddenIcon;

	return *this;
}

bool ot::NavigationTreeItemIcon::operator==(const NavigationTreeItemIcon& _other) const {
	return m_visibleIcon == _other.m_visibleIcon && m_hiddenIcon == _other.m_hiddenIcon;
}

bool ot::NavigationTreeItemIcon::operator!=(const NavigationTreeItemIcon& _other) const {
	return !(*this == _other);
}

void ot::NavigationTreeItemIcon::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Visible", JsonString(m_visibleIcon, _allocator), _allocator);
	_object.AddMember("Hidden", JsonString(m_hiddenIcon, _allocator), _allocator);
}

void ot::NavigationTreeItemIcon::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_visibleIcon = json::getString(_object, "Visible");
	m_hiddenIcon = json::getString(_object, "Hidden");
}
