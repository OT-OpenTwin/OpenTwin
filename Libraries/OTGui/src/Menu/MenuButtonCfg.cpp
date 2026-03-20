// @otlicense
// File: MenuButtonCfg.cpp
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
#include "OTCore/Logging/LogDispatcher.h"
#include "OTGui/Menu/MenuButtonCfg.h"
#include "OTGui/Menu/MenuEntryCfgFactory.h"

static ot::MenuEntryCfgFactoryRegistrar<ot::MenuButtonCfg> buttonRegistarar(ot::MenuButtonCfg::className());

std::string ot::MenuButtonCfg::toString(ButtonAction _action) {
	switch (_action) {
	case ButtonAction::NoAction: return "No Action";

	case ButtonAction::Copy: return "Copy";
	case ButtonAction::Cut: return "Cut";
	case ButtonAction::Paste: return "Paste";

	case ButtonAction::Clear: return "Clear";
	case ButtonAction::Select: return "Select";
	case ButtonAction::SelectAll: return "Select All";
	case ButtonAction::Rename: return "Rename";

	case ButtonAction::TriggerButton: return "Trigger Button";
	case ButtonAction::NotifyOwner: return "Notify Owner";
	default:
		OT_LOG_EAS("Unknown button action (" + std::to_string((int)_action) + ")");
		return "No Action";
	}
}

ot::MenuButtonCfg::ButtonAction ot::MenuButtonCfg::stringToButtonAction(const std::string& _action) {
	if (_action == MenuButtonCfg::toString(ButtonAction::NoAction)) return ButtonAction::NoAction;

	else if (_action == MenuButtonCfg::toString(ButtonAction::Copy)) return ButtonAction::Copy;
	else if (_action == MenuButtonCfg::toString(ButtonAction::Cut)) return ButtonAction::Cut;
	else if (_action == MenuButtonCfg::toString(ButtonAction::Paste)) return ButtonAction::Paste;

	else if (_action == MenuButtonCfg::toString(ButtonAction::Clear)) return ButtonAction::Clear;
	else if (_action == MenuButtonCfg::toString(ButtonAction::Select)) return ButtonAction::Select;
	else if (_action == MenuButtonCfg::toString(ButtonAction::SelectAll)) return ButtonAction::SelectAll;
	else if (_action == MenuButtonCfg::toString(ButtonAction::Rename)) return ButtonAction::Rename;

	else if (_action == MenuButtonCfg::toString(ButtonAction::TriggerButton)) return ButtonAction::TriggerButton;
	else if (_action == MenuButtonCfg::toString(ButtonAction::NotifyOwner)) return ButtonAction::NotifyOwner;
	else {
		OT_LOG_EAS("Unknown button action \"" + _action + "\"");
		return ButtonAction::NotifyOwner;
	}
}

ot::MenuButtonCfg::MenuButtonCfg() 
	: m_action(ButtonAction::NotifyOwner)
{

}

ot::MenuButtonCfg::MenuButtonCfg(const std::string& _name, const std::string& _text, const std::string& _iconPath, ButtonAction _action) 
	: MenuClickableEntryCfg(_name, _text, _iconPath), m_action(_action)
{

}

ot::MenuButtonCfg::MenuButtonCfg(const ot::ConstJsonObject& _object) {
	this->setFromJsonObject(_object);
}

ot::MenuEntryCfg* ot::MenuButtonCfg::createCopy() const {
	return new MenuButtonCfg(*this);
}

bool ot::MenuButtonCfg::isEqual(const MenuEntryCfg* _other) const
{
	const MenuButtonCfg* other = dynamic_cast<const MenuButtonCfg*>(_other);
	if (other == nullptr) {
		return false;
	}
	else
	{
		return MenuClickableEntryCfg::isEqual(_other)
			&& m_action == other->m_action
			&& m_ttbButtonName == other->m_ttbButtonName
			&& m_hidden == other->m_hidden;
	}
}

void ot::MenuButtonCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	MenuClickableEntryCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("Action", JsonString(this->toString(m_action), _allocator), _allocator);
	_object.AddMember("TriggerButton", JsonString(m_ttbButtonName, _allocator), _allocator);
	_object.AddMember("Hidden", m_hidden, _allocator);
}

void ot::MenuButtonCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	MenuClickableEntryCfg::setFromJsonObject(_object);

	m_action = this->stringToButtonAction(json::getString(_object, "Action"));
	m_ttbButtonName = json::getString(_object, "TriggerButton");
	m_hidden = json::getBool(_object, "Hidden");
}
