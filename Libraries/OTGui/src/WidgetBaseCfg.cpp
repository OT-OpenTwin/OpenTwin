// @otlicense

// OpenTwin header
#include "OTGui/WidgetBaseCfg.h"

ot::WidgetBaseCfg::WidgetBaseCfg() : m_lockFlags(LockType::All) {}

ot::WidgetBaseCfg::WidgetBaseCfg(const std::string& _name)
	: m_lockFlags(LockType::All), m_name(_name) {}

ot::WidgetBaseCfg::~WidgetBaseCfg() {}

void ot::WidgetBaseCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("ToolTip", JsonString(m_toolTip, _allocator), _allocator);
	_object.AddMember("LockFlags", JsonArray(toStringList(m_lockFlags), _allocator), _allocator);
}

void ot::WidgetBaseCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_toolTip = json::getString(_object, "ToolTip");
	m_lockFlags = stringListToLockTypes(json::getStringList(_object, "LockFlags"));
}
