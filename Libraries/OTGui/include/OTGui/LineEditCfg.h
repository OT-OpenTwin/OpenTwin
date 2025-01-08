//! @file LineEditCfg.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/WidgetBaseCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT LineEditCfg : public WidgetBaseCfg {
	public:
		LineEditCfg();
		LineEditCfg(const std::string& _name, const std::string& _text);
		LineEditCfg(const LineEditCfg&) = default;
		LineEditCfg(LineEditCfg&&) = default;
		virtual ~LineEditCfg();

		LineEditCfg& operator = (const LineEditCfg&) = default;
		LineEditCfg& operator = (LineEditCfg&&) = default;

		static std::string getLineEditCfgTypeString(void) { return "LineEditCfg"; };
		virtual std::string getWidgetBaseTypeString(void) const override { return LineEditCfg::getLineEditCfgTypeString(); };

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setReadOnly(bool _isReadOnly) { m_readOnly = _isReadOnly; };
		bool getReadOnly(void) const { return m_readOnly; };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText(void) const { return m_text; };

		void setPlaceholderText(const std::string& _text) { m_text = _text; };
		const std::string& getPlaceholderText(void) const { return m_text; };

	private:
		bool m_readOnly;
		std::string m_text;
		std::string m_placeholderText;
	};

}