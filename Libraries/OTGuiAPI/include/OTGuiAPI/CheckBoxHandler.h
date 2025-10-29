// @otlicense

//! @file CheckBoxHandler.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ActionHandler.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

// std header
#include <functional>
#include <unordered_map>

namespace ot {

	class ActionHandleConnector;

	//! @brief The CheckBoxHandler class is used to handle check box changed events.
	//! If no handler is registered for a event the corresponding virtual function will be called.
	class OT_GUIAPI_API_EXPORT CheckBoxHandler {
		OT_DECL_NOCOPY(CheckBoxHandler)
			OT_DECL_NOMOVE(CheckBoxHandler)
	public:
		CheckBoxHandler();
		virtual ~CheckBoxHandler() = default;

	protected:

		//! @brief This function will be called if no handler is registered for a check box text changed event.
		//! The default implementation will log a warning message.
		//! @param _checkBoxKey The key of the check box that was changed.
		virtual void toolBarCheckBoxChanged(const std::string& _checkBoxKey, bool _isChecked);

	public:

		//! @brief Connect a check box text changed handler.
		//! @param _callback The callback function to be called when the check box is changed.
		//! @param _checkBoxKey The key of the check box to connect the handler to.
		void connectToolBarCheckBox(const std::string& _checkBoxKey, void(*_callback)(bool));

		//! @brief Connect a check box text changed handler.
		//! @param _callback The callback function to be called when the check box text has changed.
		//! @param _checkBoxKey The key of the check box to connect the handler to.
		void connectToolBarCheckBox(const std::string& _checkBoxKey, const std::function<void(bool)>& _callback);

		//! @brief Connect a check box text changed handler.
		//! @tparam T The class type of the object.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the check box text has changed.
		//! @param _checkBoxKey The key of the check box to connect the handler to.
		template<typename T>
		void connectToolBarCheckBox(const std::string& _checkBoxKey, T* _object, void(T::* _method)(bool));

		//! @brief Connect a check box text changed handler.
		//! @tparam T The class type of the object.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the check box text has changed.
		//! @param _checkBoxKey The key of the check box to connect the handler to.
		template<typename T>
		void connectToolBarCheckBox(const std::string& _checkBoxKey, T* _object, void(T::* _method)(bool) const);

		//! @brief Disconnect a check box text changed handler.
		//! @param _checkBoxKey The key of the check box to disconnect the handler from.
		//! @return true if a handler was disconnected, false if no handler was connected for the given check box key.
		bool disconnectToolBarCheckBox(const std::string& _checkBoxKey);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Handler

	private:
		ActionHandler m_actionHandler;
		void handleToolBarCheckBoxChanged(JsonDocument& _document);

		std::unordered_map<std::string, std::function<void(bool)>> m_callbacks;
	};
}

template<typename T>
void ot::CheckBoxHandler::connectToolBarCheckBox(const std::string& _checkBoxKey, T* _object, void(T::* _method)(bool)) {
	OTAssert(m_callbacks.find(_checkBoxKey) == m_callbacks.end(), "A callback is already registered for the given tool bar check box key");
	m_callbacks.insert_or_assign(_checkBoxKey,
		[_object, _method](bool _isChecked) { (_object->*_method)(_isChecked); }
	);
}

template<typename T>
void ot::CheckBoxHandler::connectToolBarCheckBox(const std::string& _checkBoxKey, T* _object, void(T::* _method)(bool) const) {
	OTAssert(m_callbacks.find(_checkBoxKey) == m_callbacks.end(), "A callback is already registered for the given tool bar check box key");
	m_callbacks.insert_or_assign(_checkBoxKey,
		[_object, _method](bool _isChecked) { (_object->*_method)(_isChecked); }
	);
}