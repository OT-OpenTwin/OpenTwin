// @otlicense
// File: CheckBoxCfg.cpp
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
#include "OTGui/CheckBoxCfg.h"
#include "OTGui/WidgetBaseCfgFactory.h"

static ot::WidgetBaseCfgFactoryRegistrar<ot::CheckBoxCfg> g_checkBoxCfgRegistrar(ot::CheckBoxCfg::getCheckBoxCfgTypeString());

std::string ot::CheckBoxCfg::toString(CheckBoxState _state) {
	switch (_state) {
	case ot::CheckBoxCfg::Unchecked: return "Unchecked";
	case ot::CheckBoxCfg::PartiallyChecked: return "PartiallyChecked";
	case ot::CheckBoxCfg::Checked: return "Checked";
	default:
		OT_LOG_E("Unknown check box state (" + std::to_string((int)_state) + ")");
		return "Unchecked";
	}
}

ot::CheckBoxCfg::CheckBoxState ot::CheckBoxCfg::stringToCheckBoxState(const std::string& _state) {
	if (_state == toString(CheckBoxState::Unchecked)) return CheckBoxState::Unchecked;
	else if (_state == toString(CheckBoxState::PartiallyChecked)) return CheckBoxState::PartiallyChecked;
	else if (_state == toString(CheckBoxState::Checked)) return CheckBoxState::Checked;
	else {
		OT_LOG_E("Unknown check box state \"" + _state + "\"");
		return CheckBoxState::Unchecked;
	}
}

ot::CheckBoxCfg::CheckBoxCfg()
	: m_state(CheckBoxState::Unchecked)
{

}

ot::CheckBoxCfg::CheckBoxCfg(const std::string& _name, const std::string& _text, CheckBoxState _checkedState) :
	WidgetBaseCfg(_name), m_text(_text), m_state(_checkedState)
{

}

ot::CheckBoxCfg::~CheckBoxCfg() {

}

void ot::CheckBoxCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	WidgetBaseCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("State", JsonString(this->toString(m_state), _allocator), _allocator);
}

void ot::CheckBoxCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	WidgetBaseCfg::setFromJsonObject(_object);

	m_text = json::getString(_object, "Text");
	m_state = this->stringToCheckBoxState(json::getString(_object, "State"));
}