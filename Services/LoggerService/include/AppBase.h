//! \file AppBase.h
//! \author Alexander Kuester (alexk95)
//! \date January 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandleConnector.h"

// std header
#include <mutex>
#include <string>
#include <list>

class AppBase : public ot::ServiceBase, public ot::ActionHandler {
	OT_DECL_NOCOPY(AppBase)
public:
	static AppBase& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	OT_HANDLER(handleLog, AppBase, OT_ACTION_CMD_Log, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleRegister, AppBase, OT_ACTION_CMD_RegisterNewService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleDeregister, AppBase, OT_ACTION_CMD_RemoveService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleClear, AppBase, OT_ACTION_CMD_Reset, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetDebugInfo, AppBase, OT_ACTION_CMD_GetDebugInformation, ot::SECURE_MESSAGE_TYPES)

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	void notifyListeners(const ot::LogMessage& _message);

	void workerNotify(std::list<std::string> _receiver, std::string _message);

	void removeReceiver(const std::string& _receiver);

	std::list<ot::LogMessage>	m_messages;
	unsigned int				m_count;

	std::list<std::string>		m_receiver;
	std::mutex					m_receiverMutex;

	AppBase();
	virtual ~AppBase();
};