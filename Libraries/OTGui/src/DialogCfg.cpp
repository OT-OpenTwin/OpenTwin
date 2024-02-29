//! @file DialogCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/DialogCfg.h"

std::string ot::DialogCfg::flagToString(DialogFlags _flag) {
	switch (_flag)
	{
	case DialogCfg::NoFlags: return "<null>";
	case DialogCfg::CancelOnNoChange: return "CancelOnEqualValue";
	default:
		OT_LOG_EAS("Unknown dialog flag (" + std::to_string((size_t)_flag) + ")");
		return "<null>";
	}
}

ot::DialogCfg::DialogFlags ot::DialogCfg::stringToFlag(const std::string& _flag) {
	if (_flag == DialogCfg::flagToString(DialogCfg::NoFlags)) return DialogCfg::NoFlags;
	else if (_flag == DialogCfg::flagToString(DialogCfg::CancelOnNoChange)) return DialogCfg::CancelOnNoChange;
	else {
		OT_LOG_EAS("Unknown dialog flag \"" + _flag + "\"");
		return DialogCfg::NoFlags;
	}
}

std::list<std::string> ot::DialogCfg::flagsToStringList(DialogFlags _flags) {
	std::list<std::string> ret;
	if (_flags & DialogCfg::CancelOnNoChange) ret.push_back(DialogCfg::flagToString(DialogCfg::CancelOnNoChange));
	return ret;
}

ot::DialogCfg::DialogFlags ot::DialogCfg::stringListToFlags(const std::list<std::string>& _flags) {
	DialogFlags ret(NoFlags);
	for (const std::string& f : _flags) {
		ret |= DialogCfg::stringToFlag(f);
	}
	return ret;
}

void ot::DialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("Flags", JsonArray(DialogCfg::flagsToStringList(m_flags), _allocator), _allocator);
}

void ot::DialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
	m_flags = DialogCfg::stringListToFlags(json::getStringList(_object, "Flags"));
}