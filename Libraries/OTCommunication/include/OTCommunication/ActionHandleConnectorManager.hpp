// @otlicense
// File: ActionHandleConnectorManager.hpp
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
