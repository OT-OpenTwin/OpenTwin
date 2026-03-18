// @otlicense
// File: MenuButtonCfg.h
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
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/Menu/MenuClickableEntryCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT MenuButtonCfg : public MenuClickableEntryCfg {
		OT_DECL_DEFCOPY(MenuButtonCfg)
		OT_DECL_DEFMOVE(MenuButtonCfg)
	public:
		enum ButtonAction {
			NoAction,      //! @brief No action will be performed when the button is clicked.

			Copy,
			Cut,
			Paste,

			Clear,         //! @brief Clear the content of the widget (per widget implementation).
			Select,        //! @brief Select the content of the widget (per widget implementation).
			SelectAll,     //! @brief Select all content of the widget (per widget implementation).
			Rename,		   //! @brief rename the widget/context (per widget implementation).

			TriggerButton, //! @brief Trigger the button with the same name in the toolbar (if available).
			NotifyOwner    //! @brief Notify the menu owner (e.g. the widget) about the button click, so that it can handle the action (e.g. by triggering an event or calling a callback).
		};

		static std::string toString(ButtonAction _action);
		static ButtonAction stringToButtonAction(const std::string& _action);

		MenuButtonCfg();
		MenuButtonCfg(const std::string& _name, const std::string& _text, const std::string& _iconPath = std::string(), ButtonAction _action = ButtonAction::NotifyOwner);
		MenuButtonCfg(const ot::ConstJsonObject& _object);
		virtual ~MenuButtonCfg() = default;

		virtual MenuEntryCfg* createCopy() const override;
		static std::string className() { return "MenuButtonCfg"; };
		virtual std::string getClassName() const override { return MenuButtonCfg::className(); };

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		MenuButtonCfg& setButtonAction(ButtonAction _action) { m_action = _action; return *this; };
		ButtonAction getButtonAction() const { return m_action; };

		//! @brief Set the tab toolbar button to trigger when this menu button is clicked.
		//! Sets the action to TriggerButton automatically.
		//! @param _ttbButtonName Name of the button in the tab toolbar to trigger when this menu button is clicked.
		MenuButtonCfg& setTriggerButton(const std::string& _ttbButtonName) { m_ttbButtonName = _ttbButtonName; m_action = ButtonAction::TriggerButton; return *this; };
		const std::string& getTriggerButton() const { return m_ttbButtonName; };

		void setHidden(bool _hidden) { m_hidden = _hidden; };
		bool getHidden() const { return m_hidden; };

	private:
		ButtonAction m_action;
		std::string m_ttbButtonName;
		bool m_hidden = false;
	};

}