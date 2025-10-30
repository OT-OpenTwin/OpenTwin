// @otlicense
// File: CheckBoxCfg.h
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

#pragma once

// OpenTwin header
#include "OTGui/WidgetBaseCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT CheckBoxCfg : public WidgetBaseCfg {
	public:
		enum CheckBoxState {
			Unchecked,
			PartiallyChecked,
			Checked
		};
		static std::string toString(CheckBoxState _state);
		static CheckBoxState stringToCheckBoxState(const std::string& _state);

		CheckBoxCfg();
		CheckBoxCfg(const std::string& _name, const std::string& _text, CheckBoxState _checkedState = CheckBoxState::Checked);
		CheckBoxCfg(const CheckBoxCfg&) = default;
		CheckBoxCfg(CheckBoxCfg&&) = default;
		virtual ~CheckBoxCfg();

		CheckBoxCfg& operator = (const CheckBoxCfg&) = default;
		CheckBoxCfg& operator = (CheckBoxCfg&&) = default;

		static std::string getCheckBoxCfgTypeString(void) { return "CheckBoxCfg"; };
		virtual std::string getWidgetBaseTypeString(void) const override { return CheckBoxCfg::getCheckBoxCfgTypeString(); };

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText(void) const { return m_text; };

		void setChecked(bool _checked) { m_state = (_checked ? CheckBoxState::Checked : CheckBoxState::Unchecked); };
		void setCheckedState(CheckBoxState _state) { m_state = _state; };
		bool getChecked(void) const { return m_state == CheckBoxState::Checked; };
		CheckBoxState getCheckedState(void) const { return m_state; };

	private:
		std::string m_text;
		CheckBoxState m_state;
	};

}