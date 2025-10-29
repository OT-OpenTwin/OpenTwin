// @otlicense

//! @file ActionHandleConnector.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
