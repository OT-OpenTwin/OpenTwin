//! @file ButtonHandler.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin Core header
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"

// OpenTwin Gui header
#include "OTGui/ToolBarButtonCfg.h"

// OpenTwin Communication header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"

// OpenTwin GuiAPI header
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

// std header
#include <unordered_map>

namespace ot {

	//! @brief The ButtonHandler class is used to handle button click events.
	//! Register button click handlers via connectButtonHandler.
	//! If no handler is registered for a button click event the buttonClicked virtual function will be called.
	class OT_GUIAPI_API_EXPORT ButtonHandler {
		OT_DECL_NOCOPY(ButtonHandler)
		OT_DECL_NOMOVE(ButtonHandler)
		OT_DECL_ACTION_HANDLER(ButtonHandler)
	public:
		ButtonHandler() = default;
		virtual ~ButtonHandler() = default;
		
		//! @brief Connect a button click handler.
		//! @param _callback The callback function to be called when the button is clicked.
		//! @param _buttonKey The key of the button to connect the handler to.
        void connectButton(void(*_callback)(), const std::string& _buttonKey);

		//! @brief Connect a button click handler.
		//! @param _callback The callback function to be called when the button is clicked.
		//! @param _button The configuration used to determine the key of the button to connect the handler to.
		void connectButton(void(*_callback)(), const ToolBarButtonCfg& _button) { this->connectButton(_callback, _button.getFullPath()); };

		//! @brief Connect a button click handler.
		//! @param _callback The callback function to be called when the button is clicked.
		//! @param _buttonKey The key of the button to connect the handler to.
        void connectButton(const std::function<void()>& _callback, const std::string& _buttonKey);

		//! @brief Connect a button click handler.
		//! @param _callback The callback function to be called when the button is clicked.
		//! @param _button The configuration used to determine the key of the button to connect the handler to.
		void connectButton(const std::function<void()>& _callback, const ToolBarButtonCfg& _button) { this->connectButton(_callback, _button.getFullPath()); };

		//! @brief Connect a button click handler.
		//! @tparam T The class type of the object.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the button is clicked.
		//! @param _buttonKey The key of the button to connect the handler to.
        template<typename T>
        void connectButton(T* _object, void(T::* _method)(), const std::string& _buttonKey);

		//! @brief Connect a button click handler.
		//! @tparam T The class type of the object.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the button is clicked.
		//! @param _button The configuration used to determine the key of the button to connect the handler to.
		template<typename T>
		void connectButton(T* _object, void(T::* _method)(), const ToolBarButtonCfg& _button) { this->connectButton(_object, _method, _button.getFullPath()); };

		//! @brief Connect a button click handler.
		//! @tparam T The class type of the object.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the button is clicked.
		//! @param _buttonKey The key of the button to connect the handler to.
        template<typename T>
        void connectButton(T* _object, void(T::* _method)() const, const std::string& _buttonKey);

		//! @brief Connect a button click handler.
		//! @tparam T The class type of the object.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the button is clicked.
		//! @param _button The configuration used to determine the key of the button to connect the handler to.
		template<typename T>
		void connectButton(T* _object, void(T::* _method)() const, const ToolBarButtonCfg& _button) { this->connectButton(_object, _method, _button.getFullPath()); };

		//! @brief Disconnect a button click handler.
		//! @param _buttonKey The key of the button to disconnect the handler from.
		//! @return true if a handler was disconnected, false if no handler was connected for the given button key.
		bool disconnectButton(const std::string& _buttonKey);

	protected:

		//! @brief This function will be called if no handler is registered for a button click event.
		//! The default implementation will log a warning message.
		//! @param _buttonKey The key of the button that was clicked.
		virtual void buttonClicked(const std::string& _buttonKey);

        // ###########################################################################################################################################################################################################################################################################################################################
		
		// Handler

	private:
		OT_HANDLER(handleButtonClicked, ButtonHandler, OT_ACTION_CMD_MODEL_ExecuteAction, ot::SECURE_MESSAGE_TYPES)

		std::unordered_map<std::string, std::function<void()>> m_callbacks;
	};
}

template<typename T>
void ot::ButtonHandler::connectButton(T* _object, void(T::* _method)(), const std::string& _buttonKey) {
	m_callbacks.insert_or_assign(_buttonKey, 
		[_object, _method]() { (_object->*_method)(); }
	);
}

template<typename T>
void ot::ButtonHandler::connectButton(T* _object, void(T::* _method)() const, const std::string& _buttonKey) {
	m_callbacks.insert_or_assign(_buttonKey,
		[_object, _method]() { (_object->*_method)(); }
	);
}