// @otlicense
// File: ActionHandleConnector.cpp
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

// OpenTwin header
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ActionDispatcherBase.h"
#include "OTCommunication/ActionHandleConnector.h"

ot::ActionHandleConnector::ActionHandleConnector(const std::string& _actionName, const DispatchMethodType& _method, const MessageTypeFlags& _messageFlags)
	: m_messageFlags(_messageFlags), m_actionNames({ _actionName }), m_actionDispatcher(nullptr), m_method(_method)
{}

ot::ActionHandleConnector::ActionHandleConnector(const std::list<std::string>& _actionNames, const DispatchMethodType& _method, const MessageTypeFlags& _messageFlags)
	: m_messageFlags(_messageFlags), m_actionNames(_actionNames), m_actionDispatcher(nullptr), m_method(_method)
{
	m_actionNames.unique();
}

ot::ActionHandleConnector::ActionHandleConnector(const std::string& _actionName, const DispatchMethodType& _method, ActionDispatcherBase* _dispatcher, const MessageTypeFlags& _messageFlags)
	: m_messageFlags(_messageFlags), m_actionNames({ _actionName }), m_actionDispatcher(_dispatcher), m_method(_method)
{	
	OTAssertNullptr(m_actionDispatcher);
	m_actionDispatcher->add(this);
}

ot::ActionHandleConnector::ActionHandleConnector(const std::list<std::string>& _actionNames, const DispatchMethodType& _method, ActionDispatcherBase* _dispatcher, const MessageTypeFlags& _messageFlags)
	: m_messageFlags(_messageFlags), m_actionNames(_actionNames), m_actionDispatcher(_dispatcher), m_method(_method)
{
	m_actionNames.unique();

	OTAssertNullptr(m_actionDispatcher);
	m_actionDispatcher->add(this);
}

ot::ActionHandleConnector::~ActionHandleConnector() {
	if (m_actionDispatcher) {
		m_actionDispatcher->remove(this);
	}
}

bool ot::ActionHandleConnector::mayDispatch(const MessageTypeFlags& _inboundMessageType) const {
	return (_inboundMessageType & m_messageFlags);
}

std::string ot::ActionHandleConnector::forwardDispatch(JsonDocument& _document) {
	return m_method(_document);
}

void ot::ActionHandleConnector::removeFromDispatcher(void) {
	if (m_actionDispatcher) {
		m_actionDispatcher->remove(this);
		m_actionDispatcher = nullptr;
	}
}
