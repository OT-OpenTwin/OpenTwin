//! @file ToolButtonCfg.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/MenuCfg.h"
#include "OTGui/WidgetBaseCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT ToolButtonCfg : public WidgetBaseCfg {
	public:
		ToolButtonCfg();
		ToolButtonCfg(const std::string& _name, const std::string& _text, const std::string& _relativeIconPath = std::string(), const MenuCfg& _menu = MenuCfg());
		ToolButtonCfg(const ToolButtonCfg&) = default;
		ToolButtonCfg(ToolButtonCfg&&) = default;
		virtual ~ToolButtonCfg();

		ToolButtonCfg& operator = (const ToolButtonCfg&) = default;
		ToolButtonCfg& operator = (ToolButtonCfg&&) = default;

		static std::string getToolButtonCfgTypeString(void) { return "ToolButtonCfg"; };
		virtual std::string getWidgetBaseTypeString(void) const override { return ToolButtonCfg::getToolButtonCfgTypeString(); };

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText(void) const { return m_text; };

		void setIconPath(const std::string& _relativeIconPath) { m_iconPath = _relativeIconPath; };
		const std::string& getIconPath(void) const { return m_iconPath; };

	private:
		std::string m_text;
		std::string m_iconPath;
		MenuCfg m_menu;
	};

}