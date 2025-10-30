// @otlicense
// File: ActionHandleConnectorManager.h
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
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCommunication/ActionDispatcherBase.h"

// std header
#include <list>
#include <string>
#include <functional>

namespace ot {

	class ActionHandleConnector;

	//! @brief The ActionHandleConnectorManager is used for action handle connector management of an object.
	//! @tparam T Class type.
	template <class T> class ActionHandleConnectorManager {
		OT_DECL_NOCOPY(ActionHandleConnectorManager)
		OT_DECL_NOMOVE(ActionHandleConnectorManager)
		OT_DECL_NODEFAULT(ActionHandleConnectorManager)
	public:
		ActionHandleConnectorManager(T* _obj, ActionDispatcherBase* _dispatcher);
		virtual ~ActionHandleConnectorManager();

		typedef std::string(T::*HandlerMethodType)(JsonDocument& _document);

		virtual void bindHandler(T* _object, HandlerMethodType _method, const std::string& _actionName, const MessageTypeFlags& _messageFlags);
		virtual void bindHandler(T* _object, HandlerMethodType _method, const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags);

	private:
		T* m_obj;
		ActionDispatcherBase* m_dispatcher;
		std::list<ActionHandleConnector*> m_connectors;
	};

}

#include "OTCommunication/ActionHandleConnectorManager.hpp"