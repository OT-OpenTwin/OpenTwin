//! @file ActionHandlerBase.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ActionHandlerBase.h"

ot::ActionHandlerBase::ActionHandlerBase(const std::string& _actionName, ot::MessageType _messageFlags)
	:  m_messageFlags(_messageFlags)
{
	m_actionNames.push_back(_actionName);
	ActionDispatcher::instance().add(this);
}

ot::ActionHandlerBase::ActionHandlerBase(const std::list<std::string>& _actionNames, ot::MessageType _messageFlags)
	: m_messageFlags(_messageFlags), m_actionNames(_actionNames)
{
	ActionDispatcher::instance().add(this);
}

ot::ActionHandlerBase::ActionHandlerBase(const ActionHandlerBase& _other)
	: m_messageFlags(_other.m_messageFlags), m_actionNames(_other.m_actionNames)
{
	ActionDispatcher::instance().add(this, true);
}

ot::ActionHandlerBase::~ActionHandlerBase() {
	ActionDispatcher::instance().remove(this);
}

ot::ActionHandlerBase& ot::ActionHandlerBase::operator = (const ActionHandlerBase& _other) {
	ActionDispatcher::instance().remove(this);
	
	m_messageFlags = _other.m_messageFlags;
	m_actionNames = _other.m_actionNames;

	ActionDispatcher::instance().add(this, true);

	return *this;
}

bool ot::ActionHandlerBase::mayDispatch(ot::MessageType _inboundMessageType) const {
	return (_inboundMessageType & m_messageFlags);
}