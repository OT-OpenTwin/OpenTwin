// @otlicense

#pragma once

// OpenTwin header
#include "OTCommunication/ActionHandleConnectorManager.h"

#define OT_DEFAULT_ACTION_HANDLER_NAME ot_intern_actionhandleconnectormanager

namespace ot {

	template<class T>
	class ActionHandleInlineConnector {
		OT_DECL_NOCOPY(ActionHandleInlineConnector)
		OT_DECL_NOMOVE(ActionHandleInlineConnector)
		OT_DECL_NODEFAULT(ActionHandleInlineConnector)
	public:
		ActionHandleInlineConnector(T* _object, typename ActionHandleConnectorManager<T>::HandlerMethodType _method, const std::string& _actionName, ActionHandleConnectorManager<T>& _manager, const MessageTypeFlags& _messageFlags = ot::DEFAULT_MESSAGE_TYPE);
		ActionHandleInlineConnector(T* _object, typename ActionHandleConnectorManager<T>::HandlerMethodType _method, const std::list<std::string>& _actionNames, ActionHandleConnectorManager<T>& _manager, const MessageTypeFlags& _messageFlags = ot::DEFAULT_MESSAGE_TYPE);
		~ActionHandleInlineConnector() = default;
	};
}

#include "OTCommunication/ActionHandleInlineConnector.hpp"