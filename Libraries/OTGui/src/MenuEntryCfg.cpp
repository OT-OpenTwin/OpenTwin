//! @file MenuEntryCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/MenuEntryCfg.h"

std::string ot::MenuEntryCfg::toString(EntryType _type) {
	switch (_type) {
	case ot::MenuEntryCfg::Menu: return "Menu";
	case ot::MenuEntryCfg::Button: return "Button";
	case ot::MenuEntryCfg::Separator: return "Separator";
	default:
		OT_LOG_EAS("Unknown entry type (" + std::to_string((int)_type) + ")");
		return "Separator";
	}
}

ot::MenuEntryCfg::EntryType ot::MenuEntryCfg::stringToEntryType(const std::string& _type) {
	if (_type == MenuEntryCfg::toString(EntryType::Menu)) return EntryType::Menu;
	else if (_type == MenuEntryCfg::toString(EntryType::Button)) return EntryType::Button;
	else if (_type == MenuEntryCfg::toString(EntryType::Separator)) return EntryType::Separator;
	else {
		OT_LOG_EAS("Unknown entry type \"" + _type + "\"");
		return EntryType::Separator;
	}
}

const std::string& ot::MenuEntryCfg::entryTypeJsonKey(void) {
	static std::string key("EntryType");
	return key;
}

ot::MenuEntryCfg::MenuEntryCfg() {

}

ot::MenuEntryCfg::MenuEntryCfg(const MenuEntryCfg& _other) {

}

ot::MenuEntryCfg::~MenuEntryCfg() {

}

void ot::MenuEntryCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("EntryType", JsonString(this->toString(this->getMenuEntryType()), _allocator), _allocator);
}

void ot::MenuEntryCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {

}
