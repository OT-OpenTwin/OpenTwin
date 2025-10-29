// @otlicense

//! @file ToolBarButtonCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/ToolButtonCfg.h"

namespace ot {

	//! @brief Configuration class for a toolbar button.
	class OT_GUI_API_EXPORT ToolBarButtonCfg : public Serializable {
		OT_DECL_DEFCOPY(ToolBarButtonCfg)
			OT_DECL_DEFMOVE(ToolBarButtonCfg)
	public:
		explicit ToolBarButtonCfg() = default;
		explicit ToolBarButtonCfg(const std::string& _page, const std::string& _group, const ToolButtonCfg& _button);
		explicit ToolBarButtonCfg(const std::string& _page, const std::string& _group, const std::string& _buttonText, const std::string& _buttonIcon);
		explicit ToolBarButtonCfg(const std::string& _page, const std::string& _group, const std::string& _subGroup, const ToolButtonCfg& _button);
		explicit ToolBarButtonCfg(const std::string& _page, const std::string& _group, const std::string& _subGroup, const std::string& _buttonText, const std::string& _buttonIcon);
		virtual ~ToolBarButtonCfg() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		ToolBarButtonCfg& setPage(const std::string& _page) { m_page = _page; return *this; };
		const std::string& getPage() const { return m_page; };

		ToolBarButtonCfg& setGroup(const std::string& _group) { m_group = _group; return *this; };
		const std::string& getGroup() const { return m_group; };

		ToolBarButtonCfg& setSubGroup(const std::string& _subGroup) { m_subGroup = _subGroup; return *this; };
		const std::string& getSubGroup() const { return m_subGroup; };

		ToolBarButtonCfg& setButton(const ToolButtonCfg& _button) { m_button = _button; return *this; };
		ToolBarButtonCfg& setButton(ToolButtonCfg&& _button) { m_button = std::move(_button); return *this; };
		const ToolButtonCfg& getButton() const { return m_button; };

		ToolBarButtonCfg& setButtonName(const std::string& _name) { m_button.setName(_name); return *this; };
		const std::string& getButtonName() const { return m_button.getName(); };

		ToolBarButtonCfg& setButtonText(const std::string& _text) { m_button.setText(_text); return *this; };
		const std::string& getButtonText() const { return m_button.getText(); };

		ToolBarButtonCfg& setButtonKeySequence(const std::string& _keySequence) { m_button.setKeySequence(_keySequence); return *this; };
		const std::string& getButtonKeySequence() const { return m_button.getKeySequence(); };

		ToolBarButtonCfg& setButtonIconPath(const std::string& _relativeIconPath) { m_button.setIconPath(_relativeIconPath); return *this; };
		const std::string& getButtonIconPath() const { return m_button.getIconPath(); };

		ToolBarButtonCfg& setButtonToolTip(const std::string& _toolTip) { m_button.setToolTip(_toolTip); return *this; };
		const std::string& getButtonToolTip() const { return m_button.getToolTip(); };

		ToolBarButtonCfg& setButtonLockFlag(LockType _flag, bool _set = true) { m_button.setLockFlag(_flag, _set); return *this; };
		ToolBarButtonCfg& setButtonLockFlags(const LockTypes& _flags) { m_button.setLockFlags(_flags); return *this; };
		const LockTypes& getButtonLockFlags() const { return m_button.getLockFlags(); };

		ToolBarButtonCfg& setButtonMenu(const MenuCfg& _menu) { m_button.setMenu(_menu); return *this; };
		ToolBarButtonCfg& setButtonMenu(MenuCfg&& _menu) { m_button.setMenu(std::move(_menu)); return *this; };
		const MenuCfg& getButtonMenu() const { return m_button.getMenu(); };

		std::string getFullPath() const;

	private:
		std::string m_page;
		std::string m_group;
		std::string m_subGroup;
		ToolButtonCfg m_button;
	};

}