//! @file ActionHandleConnector.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ActionDispatcherBase.h"
#include "OTCommunication/ActionHandleConnector.h"

ot::ActionHandleConnector::ActionHandleConnector(const std::string& _actionName, const MessageTypeFlags& _messageFlags, const DispatchMethodType& _method, ActionDispatcherBase* _dispatcher)
	:  m_messageFlags(_messageFlags), m_actionDispatcher(_dispatcher), m_method(_method)
{
	m_actionNames.push_back(_actionName);
	m_actionDispatcher->add(this);
}

ot::ActionHandleConnector::ActionHandleConnector(const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, const DispatchMethodType& _method, ActionDispatcherBase* _dispatcher)
	: m_messageFlags(_messageFlags), m_actionNames(_actionNames), m_actionDispatcher(_dispatcher), m_method(_method)
{
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
