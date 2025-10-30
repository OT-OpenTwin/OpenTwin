// @otlicense
// File: ActionHandleInlineConnector.h
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