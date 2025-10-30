// @otlicense
// File: ButtonHandler.h
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
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/ToolBarButtonCfg.h"
#include "OTCommunication/ActionHandler.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

// std header
#include <functional>
#include <unordered_map>

namespace ot {

	class ActionHandleConnector;

	//! @brief The ButtonHandler class is used to handle button click events.
	//! Register button click handlers via connectButtonHandler.
	//! If no handler is registered for a button click event the buttonClicked virtual function will be called.
	class OT_GUIAPI_API_EXPORT ButtonHandler {
		OT_DECL_NOCOPY(ButtonHandler)
		OT_DECL_NOMOVE(ButtonHandler)
	public:	
		ButtonHandler(ActionDispatcherBase* _dispatcher = &ot::ActionDispatcher::instance());
		virtual ~ButtonHandler() = default;
		
	protected:

		//! @brief This function will be called if no handler is registered for a button click event.
		//! The default implementation will log a warning message.
		//! @param _buttonKey The key of the button that was clicked.
		virtual void toolBarButtonClicked(const std::string& _buttonKey);

	public:

		//! @brief Connect a button click handler.
		//! @param _buttonKey The key of the button to connect the handler to.
		//! @param _callback The callback function to be called when the button is clicked.
        void connectToolBarButton(const std::string& _buttonKey, void(*_callback)());

		//! @brief Connect a button click handler.
		//! @param _button The configuration used to determine the key of the button to connect the handler to.
		//! @param _callback The callback function to be called when the button is clicked.
		void connectToolBarButton(const ToolBarButtonCfg& _button, void(*_callback)()) { this->connectToolBarButton(_button.getFullPath(), _callback); };

		//! @brief Connect a button click handler.
		//! @param _buttonKey The key of the button to connect the handler to.
		//! @param _callback The callback function to be called when the button is clicked.
        void connectToolBarButton(const std::string& _buttonKey, const std::function<void()>& _callback);

		//! @brief Connect a button click handler.
		//! @param _button The configuration used to determine the key of the button to connect the handler to.
		//! @param _callback The callback function to be called when the button is clicked.
		void connectToolBarButton(const ToolBarButtonCfg& _button, const std::function<void()>& _callback) { this->connectToolBarButton(_button.getFullPath(), _callback); };

		//! @brief Connect a button click handler.
		//! This template version will accept any callable type that is not std::function<void()>.
		//! @tparam Callable The type of the callable to be connected.
		//! @tparam Enabled SFINAE helper to disable this overload for std::function<void()>.
		//! @param _buttonKey The key of the button to connect the handler to.
		//! @param _callback The callback function to be called when the button is clicked.
		template<typename Callable, typename Enabled  = std::enable_if_t<!std::is_same_v<std::decay_t<Callable>, std::function<void()>>>>
		void connectToolBarButton(const std::string& _buttonKey, Callable&& _callback);

		//! @brief Connect a button click handler.
		//! This template version will accept any callable type that is not std::function<void()>.
		//! @tparam Callable The type of the callable to be connected.
		//! @tparam Enabled SFINAE helper to disable this overload for std::function<void()>.
		//! @param _button The configuration used to determine the key of the button to connect the handler to.
		//! @param _callback The callback function to be called when the button is clicked.
		template<typename Callable, typename Enabled = std::enable_if_t<!std::is_same_v<std::decay_t<Callable>, std::function<void()>>>>
		void connectToolBarButton(const ToolBarButtonCfg& _button, Callable&& _callback) { this->connectToolBarButton(_button.getFullPath(), _callback); };

		//! @brief Connect a button click handler.
		//! @tparam T The class type of the object.
		//! @param _buttonKey The key of the button to connect the handler to.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the button is clicked.
        template<typename T>
        void connectToolBarButton(const std::string& _buttonKey, T* _object, void(T::* _method)());

		//! @brief Connect a button click handler.
		//! @tparam T The class type of the object.
		//! @param _button The configuration used to determine the key of the button to connect the handler to.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the button is clicked.
		template<typename T>
		void connectToolBarButton(const ToolBarButtonCfg& _button, T* _object, void(T::* _method)()) { this->connectToolBarButton(_button.getFullPath(), _object, _method); };

		//! @brief Connect a button click handler.
		//! @tparam T The class type of the object.
		//! @param _buttonKey The key of the button to connect the handler to.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the button is clicked.
        template<typename T>
        void connectToolBarButton(const std::string& _buttonKey, T* _object, void(T::* _method)() const);

		//! @brief Connect a button click handler.
		//! @tparam T The class type of the object.
		//! @param _button The configuration used to determine the key of the button to connect the handler to.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the button is clicked.
		template<typename T>
		void connectToolBarButton(const ToolBarButtonCfg& _button, T* _object, void(T::* _method)() const) { this->connectToolBarButton(_button.getFullPath(), _object, _method); };

		//! @brief Disconnect a button click handler.
		//! @param _buttonKey The key of the button to disconnect the handler from.
		//! @return true if a handler was disconnected, false if no handler was connected for the given button key.
		bool disconnectToolBarButton(const std::string& _buttonKey);

        // ###########################################################################################################################################################################################################################################################################################################################
		
		// Handler

	private:
		ActionHandler m_actionHandler;
		void handleToolBarButtonClicked(JsonDocument& _document);

		std::unordered_map<std::string, std::function<void()>> m_callbacks;
	};
}

template<typename Callable, typename Enabled>
inline void ot::ButtonHandler::connectToolBarButton(const std::string& _buttonKey, Callable&& _callback) {
	this->connectToolBarButton(_buttonKey, std::function<void()>(std::forward<Callable>(_callback)));
}

template<typename T>
void ot::ButtonHandler::connectToolBarButton(const std::string& _buttonKey, T* _object, void(T::* _method)()) {
	OTAssert(m_callbacks.find(_buttonKey) == m_callbacks.end(), "A callback is already registered for the given tool bar button key");
	m_callbacks.insert_or_assign(_buttonKey, 
		[_object, _method]() { (_object->*_method)(); }
	);
}

template<typename T>
void ot::ButtonHandler::connectToolBarButton(const std::string& _buttonKey, T* _object, void(T::* _method)() const) {
	OTAssert(m_callbacks.find(_buttonKey) == m_callbacks.end(), "A callback is already registered for the given tool bar button key");
	m_callbacks.insert_or_assign(_buttonKey,
		[_object, _method]() { (_object->*_method)(); }
	);
}