//! @file TableHeaderItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/TableHeaderItemCfg.h"

ot::TableHeaderItemCfg::ResizeMode ot::TableHeaderItemCfg::stringToResizeMode(const std::string& _mode) {
	if (_mode == TableHeaderItemCfg::resizeModeToString(TableHeaderItemCfg::Default)) return TableHeaderItemCfg::Default;
	else if (_mode == TableHeaderItemCfg::resizeModeToString(TableHeaderItemCfg::ResizeToContents)) return TableHeaderItemCfg::ResizeToContents;
	else if (_mode == TableHeaderItemCfg::resizeModeToString(TableHeaderItemCfg::Stretch)) return TableHeaderItemCfg::Stretch;
	else {
		OT_LOG_EAS("Unknown resize mode \"" + _mode + "\"");
		return TableHeaderItemCfg::Default;
	}
}

std::string ot::TableHeaderItemCfg::resizeModeToString(ResizeMode _mode) {
	switch (_mode) {
	case ot::TableHeaderItemCfg::Default: return "Default";
	case ot::TableHeaderItemCfg::ResizeToContents: return "Resize";
	case ot::TableHeaderItemCfg::Stretch: return "Stretch";
	default:
		OT_LOG_EAS("Unknown resize mode (" + std::to_string((int)_mode) + ")");
		return "Default";
	}
}

ot::TableHeaderItemCfg::TableHeaderItemCfg() 
	: m_resizeMode(TableHeaderItemCfg::Default)
{

}

ot::TableHeaderItemCfg::TableHeaderItemCfg(const std::string& _text, ResizeMode _resizeMode) 
	: m_text(_text), m_resizeMode(_resizeMode)
{

}

ot::TableHeaderItemCfg::TableHeaderItemCfg(const TableHeaderItemCfg& _other) 
	: m_text(_other.m_text), m_resizeMode(_other.m_resizeMode)
{

}

ot::TableHeaderItemCfg::~TableHeaderItemCfg() {

}

ot::TableHeaderItemCfg& ot::TableHeaderItemCfg::operator = (const TableHeaderItemCfg& _other) {
	if (this == &_other) return *this;

	m_text = _other.m_text;
	m_resizeMode = _other.m_resizeMode;

	return *this;
}

void ot::TableHeaderItemCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("ResizeMode", JsonString(TableHeaderItemCfg::resizeModeToString(m_resizeMode), _allocator), _allocator);
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
}

void ot::TableHeaderItemCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_text = json::getString(_object, "Text");
	m_resizeMode = TableHeaderItemCfg::stringToResizeMode(json::getString(_object, "ResizeMode"));
}
