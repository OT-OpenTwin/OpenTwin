//! @file ActionHandler.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCommunication/ActionHandlerBase.h"

// std header
#include <list>
#include <string>
#include <functional>

#define OT_ADD_HANDLE(___functionName, ___className, ___actionName, ___messageTypeFlags) ot::ActionHandler<___className> ActionHandler##___functionName##=ot::ActionHandler<___className>(___actionName, ___messageTypeFlags, this, &___className::___functionName)

//! @brief Creates a function that will be registered in the dispatch system
//! The generated function will look like this
//!		std::string <functionName>(JsonDocument& _document);
//! @param ___functionName The name of the new function
//! @param ___className The name of the class where the function is created at
//! @param ___actionName The action that should be registered in the dispatch system
#define OT_HANDLER(___functionName, ___className, ___actionName, ___messageTypeFlags) std::string ___functionName(ot::JsonDocument& _document); OT_ADD_HANDLE(___functionName, ___className, ___actionName, ___messageTypeFlags);

namespace ot {

	class OTObjectBase;

	template <class T> class __declspec(dllexport) ActionHandler : public ActionHandlerBase {
	public:
		typedef std::string(T::* ConnectorMessageRef)(JsonDocument&);

		ActionHandler();
		ActionHandler(const std::string& _actionName, ot::MessageType _messageFlags, OTObjectBase* _obj, ConnectorMessageRef _func);
		ActionHandler(const std::list<std::string>& _actionName, ot::MessageType _messageFlags, OTObjectBase* _obj, ConnectorMessageRef _func);
		ActionHandler(const ActionHandler<T>& _other);

		ActionHandler<T>& operator = (const ActionHandler<T>& _other);

		OTObjectBase* object(void) { return m_obj; }
		ConnectorMessageRef dispatchReference(void) { return m_func; }

		virtual std::string forwardDispatch(JsonDocument& _doc) override;

	private:
		OTObjectBase*       m_obj;
		ConnectorMessageRef m_func;
	};
}

template <class T>
ot::ActionHandler<T>::ActionHandler(void)
	: ActionHandlerBase(std::string(), ot::ALL_MESSAGE_TYPES), m_obj(nullptr), m_func(nullptr) 
{}

template <class T>
ot::ActionHandler<T>::ActionHandler(const std::string& _actionName, ot::MessageType _messageFlags, OTObjectBase* _obj, ConnectorMessageRef _func)
	: ActionHandlerBase(_actionName, _messageFlags), m_obj(_obj), m_func(_func)
{}

template <class T>
ot::ActionHandler<T>::ActionHandler(const std::list<std::string>& _actionNames, ot::MessageType _messageFlags, OTObjectBase* _obj, ConnectorMessageRef _func)
	: ActionHandlerBase(_actionNames, _messageFlags), m_obj(_obj), m_func(_func)
{}

template <class T>
ot::ActionHandler<T>::ActionHandler(const ActionHandler<T>& _other)
	: ActionHandlerBase(_other), m_obj(_other.m_obj), m_func(_other.m_func)
{}

template <class T>
ot::ActionHandler<T>& ot::ActionHandler<T>::operator = (const ActionHandler<T>& _other) {
	ActionHandlerBase::operator=(_other);
	m_obj = _other.m_obj;
	m_func = _other.m_func;
}

template <class T>
std::string ot::ActionHandler<T>::forwardDispatch(JsonDocument& _doc) {
	if (m_func) {
		return std::invoke(m_func, dynamic_cast<T*>(m_obj), _doc);
	}
	else {
		OTAssert(0, "Handler function not set");
		return std::string();
	}
}
