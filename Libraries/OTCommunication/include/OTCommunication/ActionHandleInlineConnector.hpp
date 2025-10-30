// @otlicense

// OpenTwin header
#include "OTCommunication/ActionHandleInlineConnector.h"

template <class T>
ot::ActionHandleInlineConnector<T>::ActionHandleInlineConnector(T* _object, typename ActionHandleConnectorManager<T>::HandlerMethodType _method, const std::string& _actionName, ActionHandleConnectorManager<T>& _manager, const MessageTypeFlags& _messageFlags) {
	_manager.bindHandler(_object, _method, _actionName, _messageFlags);
}

template <class T>
ot::ActionHandleInlineConnector<T>::ActionHandleInlineConnector(T* _object, typename ActionHandleConnectorManager<T>::HandlerMethodType _method, const std::list<std::string>& _actionNames, ActionHandleConnectorManager<T>& _manager, const MessageTypeFlags& _messageFlags) {
	_manager.bindHandler(_object, _method, _actionNames, _messageFlags);
}