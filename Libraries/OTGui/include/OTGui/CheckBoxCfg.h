// @otlicense

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