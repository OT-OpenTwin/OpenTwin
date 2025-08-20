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
#include "OTCommunication/ActionHandler.h"

// std header
#include <list>
#include <mutex>
#include <string>
#include <thread>

class AppBase : public ot::ServiceBase, public ot::AbstractLogNotifier {
	OT_DECL_NOCOPY(AppBase)
	OT_DECL_ACTION_HANDLER(AppBase)
public:
	static AppBase& instance(void);

	//! @brief Called when the a log message was created.
	virtual void log(const ot::LogMessage& _message) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	OT_HANDLER(handleLog, AppBase, OT_ACTION_CMD_Log, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRegister, AppBase, OT_ACTION_CMD_RegisterNewService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleDeregister, AppBase, OT_ACTION_CMD_RemoveService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleClear, AppBase, OT_ACTION_CMD_Reset, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetDebugInfo, AppBase, OT_ACTION_CMD_GetDebugInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSetGlobalLogFlags, AppBase, OT_ACTION_CMD_SetGlobalLogFlags, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSetCacheSize, AppBase, OT_ACTION_CMD_SetLogCacheSize, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetAllLogs, AppBase, OT_ACTION_CMD_GetAllLogs, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetUserLogs, AppBase, OT_ACTION_CMD_GetUserLogs, ot::ALL_MESSAGE_TYPES)

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	void updateBufferSizeFromLogFlags(const ot::LogFlags& _flags);

private:
	void appendLogMessage(ot::LogMessage&& _message);

	void workerNotify();

	void removeReceiver(const std::string& _receiver);

	void resizeBuffer(void);

	std::mutex                  m_newMessageMutex;
	std::list<ot::LogMessage>   m_newMessages;

	std::mutex					m_mutex;
	std::thread*                m_notifyThread;
	std::atomic_bool            m_notifyThreadRunning;

	size_t						m_bufferSize;
	std::list<ot::LogMessage>	m_messages;
	size_t						m_count;

	std::list<std::string>		m_receiver;

	AppBase();
	virtual ~AppBase();
};