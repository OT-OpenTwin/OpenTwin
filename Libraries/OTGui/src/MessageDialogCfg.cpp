// @otlicense

//! @file MessageDialogCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/MessageDialogCfg.h"

std::string ot::MessageDialogCfg::toString(BasicButton _button) {
	switch (_button)
	{
	case ot::MessageDialogCfg::NoButtons: return "<null>";
	case ot::MessageDialogCfg::Ok: return "Ok";
	case ot::MessageDialogCfg::Save: return "Save";
	case ot::MessageDialogCfg::SaveAll: return "SaveAll";
	case ot::MessageDialogCfg::Open: return "Open";
	case ot::MessageDialogCfg::Yes: return "Yes";
	case ot::MessageDialogCfg::YesToAll: return "YesToAll";
	case ot::MessageDialogCfg::No: return "No";
	case ot::MessageDialogCfg::NoToAll: return "NoToAll";
	case ot::MessageDialogCfg::Abort: return "Abort";
	case ot::MessageDialogCfg::Retry: return "Retry";
	case ot::MessageDialogCfg::Ignore: return "Ignore";
	case ot::MessageDialogCfg::Close: return "Close";
	case ot::MessageDialogCfg::Cancel: return "Cancel";
	case ot::MessageDialogCfg::Discard: return "Discard";
	case ot::MessageDialogCfg::Help: return "Help";
	case ot::MessageDialogCfg::Apply: return "Apply";
	case ot::MessageDialogCfg::Reset: return "Reset";
	case ot::MessageDialogCfg::RestoreDefaults: return "RestoreDefaults";
	default:
		OT_LOG_EAS("Unknown button (" + std::to_string((int)_button) + ")");
		return "<null>";
	}
}

ot::MessageDialogCfg::BasicButton ot::MessageDialogCfg::stringToButton(const std::string& _button) {
	if (_button == toString(MessageDialogCfg::NoButtons)) { return MessageDialogCfg::NoButtons; }
	else if (_button == toString(MessageDialogCfg::Ok)) { return MessageDialogCfg::Ok; }
	else if (_button == toString(MessageDialogCfg::Save)) { return MessageDialogCfg::Save; }
	else if (_button == toString(MessageDialogCfg::SaveAll)) { return MessageDialogCfg::SaveAll; }
	else if (_button == toString(MessageDialogCfg::Open)) { return MessageDialogCfg::Open; }
	else if (_button == toString(MessageDialogCfg::Yes)) { return MessageDialogCfg::Yes; }
	else if (_button == toString(MessageDialogCfg::YesToAll)) { return MessageDialogCfg::YesToAll; }
	else if (_button == toString(MessageDialogCfg::No)) { return MessageDialogCfg::No; }
	else if (_button == toString(MessageDialogCfg::NoToAll)) { return MessageDialogCfg::NoToAll; }
	else if (_button == toString(MessageDialogCfg::Abort)) { return MessageDialogCfg::Abort; }
	else if (_button == toString(MessageDialogCfg::Retry)) { return MessageDialogCfg::Retry; }
	else if (_button == toString(MessageDialogCfg::Ignore)) { return MessageDialogCfg::Ignore; }
	else if (_button == toString(MessageDialogCfg::Close)) { return MessageDialogCfg::Close; }
	else if (_button == toString(MessageDialogCfg::Cancel)) { return MessageDialogCfg::Cancel; }
	else if (_button == toString(MessageDialogCfg::Discard)) { return MessageDialogCfg::Discard; }
	else if (_button == toString(MessageDialogCfg::Help)) { return MessageDialogCfg::Help; }
	else if (_button == toString(MessageDialogCfg::Apply)) { return MessageDialogCfg::Apply; }
	else if (_button == toString(MessageDialogCfg::Reset)) { return MessageDialogCfg::Reset; }
	else if (_button == toString(MessageDialogCfg::RestoreDefaults)) { return MessageDialogCfg::RestoreDefaults; }
	else {
		OT_LOG_EAS("Unknown button \"" + _button + "\"");
		return MessageDialogCfg::NoButtons;
	}
}

std::list<std::string> ot::MessageDialogCfg::toStringList(const BasicButtons& _buttons) {
	std::list<std::string> ret;
	if (_buttons & MessageDialogCfg::Ok) ret.push_back(toString(MessageDialogCfg::Ok));
	if (_buttons & MessageDialogCfg::Save) ret.push_back(toString(MessageDialogCfg::Save));
	if (_buttons & MessageDialogCfg::SaveAll) ret.push_back(toString(MessageDialogCfg::SaveAll));
	if (_buttons & MessageDialogCfg::Open) ret.push_back(toString(MessageDialogCfg::Open));
	if (_buttons & MessageDialogCfg::Yes) ret.push_back(toString(MessageDialogCfg::Yes));
	if (_buttons & MessageDialogCfg::YesToAll) ret.push_back(toString(MessageDialogCfg::YesToAll));
	if (_buttons & MessageDialogCfg::No) ret.push_back(toString(MessageDialogCfg::No));
	if (_buttons & MessageDialogCfg::NoToAll) ret.push_back(toString(MessageDialogCfg::NoToAll));
	if (_buttons & MessageDialogCfg::Abort) ret.push_back(toString(MessageDialogCfg::Abort));
	if (_buttons & MessageDialogCfg::Retry) ret.push_back(toString(MessageDialogCfg::Retry));
	if (_buttons & MessageDialogCfg::Ignore) ret.push_back(toString(MessageDialogCfg::Ignore));
	if (_buttons & MessageDialogCfg::Close) ret.push_back(toString(MessageDialogCfg::Close));
	if (_buttons & MessageDialogCfg::Cancel) ret.push_back(toString(MessageDialogCfg::Cancel));
	if (_buttons & MessageDialogCfg::Discard) ret.push_back(toString(MessageDialogCfg::Discard));
	if (_buttons & MessageDialogCfg::Help) ret.push_back(toString(MessageDialogCfg::Help));
	if (_buttons & MessageDialogCfg::Apply) ret.push_back(toString(MessageDialogCfg::Apply));
	if (_buttons & MessageDialogCfg::Reset) ret.push_back(toString(MessageDialogCfg::Reset));
	if (_buttons & MessageDialogCfg::RestoreDefaults) ret.push_back(toString(MessageDialogCfg::RestoreDefaults));
	return ret;
}

ot::MessageDialogCfg::BasicButtons ot::MessageDialogCfg::stringListToButtons(const std::list<std::string>& _buttons) {
	ot::MessageDialogCfg::BasicButtons ret = NoButtons;
	for (const std::string& b : _buttons) {
		ret |= stringToButton(b);
	}
	return ret;
}

std::string ot::MessageDialogCfg::iconToString(BasicIcon _icon) {
	switch (_icon)
	{
	case ot::MessageDialogCfg::NoIcon: return "NoIcon";
	case ot::MessageDialogCfg::Information: return "Information";
	case ot::MessageDialogCfg::Question: return "Question";
	case ot::MessageDialogCfg::Warning: return "Warning";
	case ot::MessageDialogCfg::Critical: return "Critical";
	default:
		OT_LOG_EAS("Unknown icon (" + std::to_string((int)_icon) + ")");
		return "NoIcon";
	}
}

ot::MessageDialogCfg::BasicIcon ot::MessageDialogCfg::stringToIcon(const std::string& _icon) {
	if (_icon == iconToString(MessageDialogCfg::NoIcon)) return MessageDialogCfg::NoIcon;
	else if (_icon == iconToString(MessageDialogCfg::Information)) return MessageDialogCfg::Information;
	else if (_icon == iconToString(MessageDialogCfg::Question)) return MessageDialogCfg::Question;
	else if (_icon == iconToString(MessageDialogCfg::Warning)) return MessageDialogCfg::Warning;
	else if (_icon == iconToString(MessageDialogCfg::Critical)) return MessageDialogCfg::Critical;
	else {
		OT_LOG_EAS("Unknown icon \"" + _icon + "\"");
		return MessageDialogCfg::NoIcon;
	}
}

ot::MessageDialogCfg::MessageDialogCfg() : m_buttons(MessageDialogCfg::Ok), m_icon(MessageDialogCfg::NoIcon) {

}

ot::MessageDialogCfg::~MessageDialogCfg() {

}

void ot::MessageDialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	DialogCfg::addToJsonObject(_object, _allocator);
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("Detailed", JsonString(m_detailedText, _allocator), _allocator);
	_object.AddMember("Icon", JsonString(this->iconToString(m_icon), _allocator), _allocator);
	_object.AddMember("Buttons", JsonArray(this->toStringList(m_buttons), _allocator), _allocator);
}

void ot::MessageDialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	DialogCfg::setFromJsonObject(_object);
	m_text = json::getString(_object, "Text");
	m_detailedText = json::getString(_object, "Detailed");
	m_icon = this->stringToIcon(json::getString(_object, "Icon"));
	m_buttons = this->stringListToButtons(json::getStringList(_object, "Buttons"));
}