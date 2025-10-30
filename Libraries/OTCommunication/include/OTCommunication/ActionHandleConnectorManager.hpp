// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionHandleConnector.h"
#include "OTCommunication/ActionHandleConnectorManager.h"

template <class T>
ot::ActionHandleConnectorManager<T>::ActionHandleConnectorManager(T* _obj, ActionDispatcherBase* _dispatcher)
	: m_obj(_obj), m_dispatcher(_dispatcher)
{
	OTAssertNullptr(m_obj);
	OTAssertNullptr(m_dispatcher);
}

template <class T>
ot::ActionHandleConnectorManager<T>::~ActionHandleConnectorManager() {
	for (ActionHandleConnector* connector : m_connectors) {
		delete connector;
	}
}

template<class T>
inline void ot::ActionHandleConnectorManager<T>::bindHandler(T* _object, HandlerMethodType _method, const std::string& _actionName, const MessageTypeFlags& _messageFlags) {
	m_connectors.push_back(new ActionHandleConnector(_actionName, ActionHandleConnector::DispatchMethodType(std::bind(_method, _object, std::placeholders::_1)), m_dispatcher, _messageFlags));
}

template<class T>
inline void ot::ActionHandleConnectorManager<T>::bindHandler(T* _object, HandlerMethodType _method, const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags) {
	m_connectors.push_back(new ActionHandleConnector(_actionNames, ActionHandleConnector::DispatchMethodType(std::bind(_method, _object, std::placeholders::_1)), m_dispatcher, _messageFlags));
}
