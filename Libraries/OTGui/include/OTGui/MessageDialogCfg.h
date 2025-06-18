//! @file MessageDialogCfg.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTGui/DialogCfg.h"

// std header
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT MessageDialogCfg : public DialogCfg {
	public:
		enum BasicButton {
			NoButtons       = 0 << 0,
			Ok              = 1 << 0,
			Save            = 1 << 1,
			SaveAll         = 1 << 2,
			Open            = 1 << 3,
			Yes             = 1 << 4,
			YesToAll        = 1 << 5,
			No              = 1 << 6,
			NoToAll         = 1 << 7,
			Abort           = 1 << 8,
			Retry           = 1 << 9,
			Ignore          = 1 << 10,
			Close           = 1 << 11,
			Cancel          = 1 << 12,
			Discard         = 1 << 13,
			Help            = 1 << 14,
			Apply           = 1 << 15,
			Reset           = 1 << 16,
			RestoreDefaults = 1 << 17
		};
		typedef Flags<BasicButton> BasicButtons;

		enum BasicIcon {
			NoIcon,
			Information,
			Question,
			Warning,
			Critical
		};

		static std::string toString(BasicButton _button);
		static BasicButton stringToButton(const std::string& _button);
		static std::list<std::string> toStringList(const BasicButtons& _buttons);
		static BasicButtons stringListToButtons(const std::list<std::string>& _buttons);

		static std::string iconToString(BasicIcon _icon);
		static BasicIcon stringToIcon(const std::string& _icon);

		MessageDialogCfg();
		virtual ~MessageDialogCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText(void) const { return m_text; };

		void setButton(BasicButton _button, bool _active = true) { m_buttons.setFlag(_button, _active); };
		void setButtons(BasicButtons _buttons) { m_buttons = _buttons; };
		const BasicButtons& getButtons(void) const { return m_buttons; };

		void setIcon(BasicIcon _icon) { m_icon = _icon; };
		BasicIcon getIcon(void) const { return m_icon; };

	private:
		std::string m_text;
		BasicButtons m_buttons;
		BasicIcon m_icon;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::MessageDialogCfg::BasicButton)