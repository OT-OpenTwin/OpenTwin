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
		OT_DECL_DEFCOPY(ToolButtonCfg)
		OT_DECL_DEFMOVE(ToolButtonCfg)
	public:
		ToolButtonCfg();
		explicit ToolButtonCfg(const std::string& _name, const std::string& _text, const std::string& _relativeIconPath = std::string(), const MenuCfg& _menu = MenuCfg());
		virtual ~ToolButtonCfg();

		static std::string getToolButtonCfgTypeString() { return "ToolButtonCfg"; };
		virtual std::string getWidgetBaseTypeString() const override { return ToolButtonCfg::getToolButtonCfgTypeString(); };

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText() const { return m_text; };

		void setIconPath(const std::string& _relativeIconPath) { m_iconPath = _relativeIconPath; };
		const std::string& getIconPath() const { return m_iconPath; };

		void setKeySequence(const std::string& _keySequence) { m_keySequence = _keySequence; };
		const std::string& getKeySequence() const { return m_keySequence; };

		void setMenu(const MenuCfg& _menu) { m_menu = _menu; };
		void setMenu(MenuCfg&& _menu) { m_menu = std::move(_menu); };
		const MenuCfg& getMenu() const { return m_menu; };

	private:
		std::string m_text;
		std::string m_iconPath;
		std::string m_keySequence;
		MenuCfg m_menu;
	};

}