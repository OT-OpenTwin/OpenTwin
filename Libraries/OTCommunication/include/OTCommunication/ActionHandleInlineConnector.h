//! @file ActionHandleInlineConnector.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCommunication/ActionHandleConnectorManager.h"

#define OT_INTERN_DEFAULT_ACTION_HANDLER_NAME ot_intern_actionhandleconnectormanager

namespace ot {

	template<class T>
	class ActionHandleInlineConnector {
		OT_DECL_NOCOPY(ActionHandleInlineConnector)
		OT_DECL_NOMOVE(ActionHandleInlineConnector)
		OT_DECL_NODEFAULT(ActionHandleInlineConnector)
	public:
		ActionHandleInlineConnector(T* _object, typename ActionHandleConnectorManager<T>::HandlerMethodType _method, const std::string& _actionName, MessageType _messageType, ActionHandleConnectorManager<T>& _manager);
		ActionHandleInlineConnector(T* _object, typename ActionHandleConnectorManager<T>::HandlerMethodType _method, const std::list<std::string>& _actionNames, MessageType _messageType, ActionHandleConnectorManager<T>& _manager);
		~ActionHandleInlineConnector() = default;
	};
}

#include "OTCommunication/ActionHandleInlineConnector.hpp"