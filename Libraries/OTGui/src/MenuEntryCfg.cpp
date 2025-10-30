// @otlicense
// File: MenuEntryCfg.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
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

ot::MenuEntryCfg& ot::MenuEntryCfg::operator=(const MenuEntryCfg& _other) {
	return *this;
}

void ot::MenuEntryCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("EntryType", JsonString(this->toString(this->getMenuEntryType()), _allocator), _allocator);
}

void ot::MenuEntryCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {

}
