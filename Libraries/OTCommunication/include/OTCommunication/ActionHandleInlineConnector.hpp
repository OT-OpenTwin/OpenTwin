//! @file ActionHandleInlineConnector.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCommunication/ActionHandleInlineConnector.h"

template <class T>
ot::ActionHandleInlineConnector<T>::ActionHandleInlineConnector(T* _object, typename ActionHandleConnectorManager<T>::HandlerMethodType _method, const std::string& _actionName, MessageType _messageType, ActionHandleConnectorManager<T>& _manager) {
	_manager.bindHandler(_object, _method, _actionName, _messageType);
}

template <class T>
ot::ActionHandleInlineConnector<T>::ActionHandleInlineConnector(T* _object, typename ActionHandleConnectorManager<T>::HandlerMethodType _method, const std::list<std::string>& _actionNames, MessageType _messageType, ActionHandleConnectorManager<T>& _manager) {
	_manager.bindHandler(_object, _method, _actionNames, _messageType);
}