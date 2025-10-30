// @otlicense

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

	//! @brief The LineEditHandler class is used to handle line edit changed events.
	//! If no handler is registered for a event the corresponding virtual function will be called.
	class OT_GUIAPI_API_EXPORT LineEditHandler {
		OT_DECL_NOCOPY(LineEditHandler)
		OT_DECL_NOMOVE(LineEditHandler)
	public:
		LineEditHandler();
		virtual ~LineEditHandler() = default;

	protected:

		//! @brief This function will be called if no handler is registered for a line edit text changed event.
		//! The default implementation will log a warning message.
		//! @param _lineEditKey The key of the line edit that was changed.
		virtual void toolBarLineEditTextChanged(const std::string& _lineEditKey, const std::string& _newText);

	public:

		//! @brief Connect a line edit text changed handler.
		//! @param _callback The callback function to be called when the line edit is changed.
		//! @param _lineEditKey The key of the line edit to connect the handler to.
		void connectToolBarLineEdit(const std::string& _lineEditKey, void(*_callback)(const std::string&));

		//! @brief Connect a line edit text changed handler.
		//! @param _callback The callback function to be called when the line edit text has changed.
		//! @param _lineEditKey The key of the line edit to connect the handler to.
		void connectToolBarLineEdit(const std::string& _lineEditKey, const std::function<void(const std::string&)>& _callback);

		//! @brief Connect a line edit text changed handler.
		//! @tparam T The class type of the object.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the line edit text has changed.
		//! @param _lineEditKey The key of the line edit to connect the handler to.
		template<typename T>
		void connectToolBarLineEdit(const std::string& _lineEditKey, T* _object, void(T::* _method)(const std::string&));

		//! @brief Connect a line edit text changed handler.
		//! @tparam T The class type of the object.
		//! @param _object The object to connect the handler to.
		//! @param _method The member function to be called when the line edit text has changed.
		//! @param _lineEditKey The key of the line edit to connect the handler to.
		template<typename T>
		void connectToolBarLineEdit(const std::string& _lineEditKey, T* _object, void(T::* _method)(const std::string&) const);

		//! @brief Disconnect a line edit text changed handler.
		//! @param _lineEditKey The key of the line edit to disconnect the handler from.
		//! @return true if a handler was disconnected, false if no handler was connected for the given line edit key.
		bool disconnectToolBarLineEdit(const std::string& _lineEditKey);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Handler

	private:
		ActionHandler m_actionHandler;
		void handleToolBarLineEditChanged(JsonDocument& _document);

		std::unordered_map<std::string, std::function<void(const std::string&)>> m_callbacks;
	};
}

template<typename T>
void ot::LineEditHandler::connectToolBarLineEdit(const std::string& _lineEditKey, T* _object, void(T::* _method)(const std::string&)) {
	OTAssert(m_callbacks.find(_lineEditKey) == m_callbacks.end(), "A callback is already registered for the given tool bar line edit key");
	m_callbacks.insert_or_assign(_lineEditKey,
		[_object, _method](const std::string& _newText) { (_object->*_method)(_newText); }
	);
}

template<typename T>
void ot::LineEditHandler::connectToolBarLineEdit(const std::string& _lineEditKey, T* _object, void(T::* _method)(const std::string&) const) {
	OTAssert(m_callbacks.find(_lineEditKey) == m_callbacks.end(), "A callback is already registered for the given tool bar line edit key");
	m_callbacks.insert_or_assign(_lineEditKey,
		[_object, _method](const std::string& _newText) { (_object->*_method)(_newText); }
	);
}