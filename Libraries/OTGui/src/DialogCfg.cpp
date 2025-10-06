//! @file DialogCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/DialogCfg.h"

std::string ot::DialogCfg::toString(DialogFlag _flag) {
	switch (_flag)
	{
	case DialogCfg::NoFlags: return "<null>";
	case DialogCfg::CancelOnNoChange: return "CancelOnNoChange";
	case DialogCfg::MoveGrabAnywhere: return "MoveGrabAnywhere";
	case DialogCfg::RecenterOnF11: return "RecenterOnF11";
	default:
		OT_LOG_EAS("Unknown dialog flag (" + std::to_string((size_t)_flag) + ")");
		return "<null>";
	}
}

ot::DialogCfg::DialogFlag ot::DialogCfg::stringToFlag(const std::string& _flag) {
	if (_flag == DialogCfg::toString(DialogCfg::NoFlags)) return DialogCfg::NoFlags;
	else if (_flag == DialogCfg::toString(DialogCfg::CancelOnNoChange)) return DialogCfg::CancelOnNoChange;
	else if (_flag == DialogCfg::toString(DialogCfg::MoveGrabAnywhere)) return DialogCfg::MoveGrabAnywhere;
	else if (_flag == DialogCfg::toString(DialogCfg::RecenterOnF11)) return DialogCfg::RecenterOnF11;
	else {
		OT_LOG_EAS("Unknown dialog flag \"" + _flag + "\"");
		return DialogCfg::NoFlags;
	}
}

std::list<std::string> ot::DialogCfg::toStringList(DialogFlags _flags) {
	std::list<std::string> ret;
	if (_flags & DialogCfg::CancelOnNoChange) ret.push_back(DialogCfg::toString(DialogCfg::CancelOnNoChange));
	if (_flags & DialogCfg::MoveGrabAnywhere) ret.push_back(DialogCfg::toString(DialogCfg::MoveGrabAnywhere));
	if (_flags & DialogCfg::RecenterOnF11) ret.push_back(DialogCfg::toString(DialogCfg::RecenterOnF11));
	return ret;
}

ot::DialogCfg::DialogFlags ot::DialogCfg::stringListToFlags(const std::list<std::string>& _flags) {
	DialogFlags ret(NoFlags);
	for (const std::string& f : _flags) {
		ret |= DialogCfg::stringToFlag(f);
	}
	return ret;
}

ot::DialogCfg::DialogCfg(DialogFlags _flags)
	: m_flags(_flags), m_initialSize(-1, -1), m_minSize(-1, -1), m_maxSize(-1, -1)
{}

ot::DialogCfg::DialogCfg(const std::string& _title, DialogFlags _flags)
	: m_flags(_flags), m_title(_title), m_initialSize(-1, -1), m_minSize(-1, -1), m_maxSize(-1, -1)
{}

void ot::DialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("Flags", JsonArray(DialogCfg::toStringList(m_flags), _allocator), _allocator);

	JsonObject iniSizeObj;
	m_initialSize.addToJsonObject(iniSizeObj, _allocator);
	_object.AddMember("IniSize", iniSizeObj, _allocator);

	JsonObject minSizeObj;
	m_minSize.addToJsonObject(minSizeObj, _allocator);
	_object.AddMember("MinSize", minSizeObj, _allocator);

	JsonObject maxSizeObj;
	m_maxSize.addToJsonObject(maxSizeObj, _allocator);
	_object.AddMember("MaxSize", maxSizeObj, _allocator);
}

void ot::DialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
	m_flags = DialogCfg::stringListToFlags(json::getStringList(_object, "Flags"));
	m_initialSize.setFromJsonObject(json::getObject(_object, "IniSize"));
	m_minSize.setFromJsonObject(json::getObject(_object, "MinSize"));
	m_maxSize.setFromJsonObject(json::getObject(_object, "MaxSize"));
}