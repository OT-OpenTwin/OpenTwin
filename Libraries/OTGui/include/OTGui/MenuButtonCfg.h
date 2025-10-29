// @otlicense

//! @file MenuButtonCfg.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/MenuClickableEntryCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT MenuButtonCfg : public MenuClickableEntryCfg {
	public:
		enum ButtonAction {
			NotifyOwner,
			Clear
		};

		static std::string toString(ButtonAction _action);
		static ButtonAction stringToButtonAction(const std::string& _action);

		MenuButtonCfg();
		MenuButtonCfg(const std::string& _name, const std::string& _text, const std::string& _iconPath = std::string(), ButtonAction _action = ButtonAction::NotifyOwner);
		MenuButtonCfg(const MenuButtonCfg& _other);
		MenuButtonCfg(const ot::ConstJsonObject& _object);
		virtual ~MenuButtonCfg();

		MenuButtonCfg& operator = (const MenuButtonCfg&) = delete;

		virtual MenuEntryCfg* createCopy(void) const override;
		virtual EntryType getMenuEntryType(void) const override { return MenuEntryCfg::Button; };

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setButtonAction(ButtonAction _action) { m_action = _action; };
		ButtonAction getButtonAction(void) const { return m_action; };

	private:
		ButtonAction m_action;
	};

}