//! @file ActionHandleConnector.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCommunication/ActionHandler.h"
#include "OTCommunication/ActionHandleConnectorBase.h"

// std header
#include <list>
#include <string>
#include <functional>

#define OT_ADD_HANDLER(___functionName, ___className, ___actionName, ___messageTypeFlags) ot::ActionHandleConnector<___className> ActionHandleConnector##___functionName##=ot::ActionHandleConnector<___className>(___actionName, ___messageTypeFlags, this, &___className::___functionName)

//! @brief Creates a function that will be registered in the dispatch system.
//! The generated function will look like this:
//!		std::string <functionName>(JsonDocument& _document);
//! @param ___functionName The name of the new function.
//! @param ___className The name of the class where the function is created at.
//! @param ___actionName The action that should be registered in the dispatch system.
#define OT_HANDLER(___functionName, ___className, ___actionName, ___messageTypeFlags) std::string ___functionName(ot::JsonDocument& _document); OT_ADD_HANDLER(___functionName, ___className, ___actionName, ___messageTypeFlags);

namespace ot {

	//! @class ActionHandleConnector
	//! @brief The ActionHandleConnector is used as a connector between the [\ref ActionDispatcher] and a function in a [\ref ActionHandler] object.
	template <class T> class __declspec(dllexport) ActionHandleConnector : public ActionHandleConnectorBase {
	public:
		typedef std::string(T::* ConnectorMessageRef)(JsonDocument&);

		ActionHandleConnector();
		ActionHandleConnector(const std::string& _actionName, ot::MessageType _messageFlags, ActionHandler* _handler, ConnectorMessageRef _handlerFunction);
		ActionHandleConnector(const std::list<std::string>& _actionNames, ot::MessageType _messageFlags, ActionHandler* _handler, ConnectorMessageRef _handlerFunction);
		ActionHandleConnector(const ActionHandleConnector<T>& _other);

		ActionHandleConnector<T>& operator = (const ActionHandleConnector<T>& _other);

		ActionHandler* getHandler(void) { return m_obj; }
		ConnectorMessageRef getDispatchReference(void) { return m_func; }

		virtual std::string forwardDispatch(JsonDocument& _doc) override;

	private:
		ActionHandler*       m_obj;
		ConnectorMessageRef m_func;
	};
}

template <class T>
ot::ActionHandleConnector<T>::ActionHandleConnector(void)
	: ActionHandleConnectorBase(std::string(), ot::ALL_MESSAGE_TYPES), m_obj(nullptr), m_func(nullptr)
{}

template <class T>
ot::ActionHandleConnector<T>::ActionHandleConnector(const std::string& _actionName, ot::MessageType _messageFlags, ActionHandler* _obj, ConnectorMessageRef _func)
	: ActionHandleConnectorBase(_actionName, _messageFlags), m_obj(_obj), m_func(_func)
{}

template <class T>
ot::ActionHandleConnector<T>::ActionHandleConnector(const std::list<std::string>& _actionNames, ot::MessageType _messageFlags, ActionHandler* _obj, ConnectorMessageRef _func)
	: ActionHandleConnectorBase(_actionNames, _messageFlags), m_obj(_obj), m_func(_func)
{}

template <class T>
ot::ActionHandleConnector<T>::ActionHandleConnector(const ActionHandleConnector<T>& _other)
	: ActionHandleConnectorBase(_other), m_obj(_other.m_obj), m_func(_other.m_func)
{}

template <class T>
ot::ActionHandleConnector<T>& ot::ActionHandleConnector<T>::operator = (const ActionHandleConnector<T>& _other) {
	ActionHandleConnectorBase::operator=(_other);
	m_obj = _other.m_obj;
	m_func = _other.m_func;
}

template <class T>
std::string ot::ActionHandleConnector<T>::forwardDispatch(JsonDocument& _doc) {
	if (m_func) {
		return std::invoke(m_func, dynamic_cast<T*>(m_obj), _doc);
	}
	else {
		OTAssert(0, "Handler function not set");
		return std::string();
	}
}
