//! @file MessageDialogCfg.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTGui/DialogCfg.h"

// std header
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT MessageDialogCfg : public DialogCfg {
	public:
		enum BasicButton {
			NoButtons       = 0x00000,
			Ok              = 0x00001,
			Save            = 0x00002,
			SaveAll         = 0x00004,
			Open            = 0x00008,
			Yes             = 0x00010,
			YesToAll        = 0x00020,
			No              = 0x00040,
			NoToAll         = 0x00080,
			Abort           = 0x00100,
			Retry           = 0x00200,
			Ignore          = 0x00400,
			Close           = 0x00800,
			Cancel          = 0x01000,
			Discard         = 0x02000,
			Help            = 0x04000,
			Apply           = 0x08000,
			Reset           = 0x10000,
			RestoreDefaults = 0x20000
		};

		enum BasicIcon {
			NoIcon,
			Information,
			Question,
			Warning,
			Critical
		};

		static std::string buttonToString(BasicButton _button);
		static BasicButton stringToButton(const std::string& _button);
		static std::list<std::string> buttonsToStringList(BasicButton _buttons);
		static BasicButton stringListToButtons(const std::list<std::string>& _buttons);

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
		const std::string& text(void) const { return m_text; };

		void setButtons(BasicButton _buttons) { m_buttons = _buttons; };
		BasicButton buttons(void) const { return m_buttons; };

		void setIcon(BasicIcon _icon) { m_icon = _icon; };
		BasicIcon icon(void) const { return m_icon; };

	private:
		std::string m_text;
		BasicButton m_buttons;
		BasicIcon m_icon;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::MessageDialogCfg::BasicButton)