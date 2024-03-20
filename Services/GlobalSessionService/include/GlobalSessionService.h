//! @file GlobalSessionService.h
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/OTObjectBase.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTServiceFoundation/IDManager.h"

// C++ header
#include <string>
#include <list>
#include <map>
#include <mutex>

class LocalSessionService;

class GlobalSessionService : public ot::OTObjectBase, public ot::ServiceBase {
public:
	static GlobalSessionService * instance(void);
	static bool hasInstance(void);
	static void deleteInstance(void);

	// ###################################################################################################

	// Properties
	OT_PROPERTY_REF(std::string, url, setUrl, url)
	OT_PROPERTY_REF(std::string, databaseUrl, setDatabaseUrl, databaseUrl)
	OT_PROPERTY_REF(std::string, authorizationUrl, setAuthorizationUrl, authorizationUrl)
	OT_PROPERTY_REF(std::string, globalDirectoryUrl, setGlobalDirectoryUrl, globalDirectoryUrl)
	OT_PROPERTY_V(bool, healthCheckRunning, setHealthCheckIsRunning, isHealthCheckRunning, private)

	// ###################################################################################################

	// Service handling

	bool addSessionService(LocalSessionService& _service);

private:

	// ###################################################################################################

	// Handler

	OT_HANDLER(handleGetDBUrl, GlobalSessionService, OT_ACTION_CMD_GetDatabaseUrl, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleGetDBandAuthURL, GlobalSessionService, OT_ACTION_CMD_GetDBandAuthServerUrl, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleCreateSession, GlobalSessionService, OT_ACTION_CMD_CreateNewSession, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleCheckProjectOpen, GlobalSessionService, OT_ACTION_CMD_IsProjectOpen, ot::ALL_MESSAGE_TYPES);

	OT_HANDLER(handleRegisterSessionService, GlobalSessionService, OT_ACTION_CMD_RegisterNewSessionService, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleShutdownSession, GlobalSessionService, OT_ACTION_CMD_ShutdownSessionCompleted, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleForceHealthcheck, GlobalSessionService, OT_ACTION_CMD_ForceHealthCheck, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleNewGlobalDirectoryService, GlobalSessionService, OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, ot::SECURE_MESSAGE_TYPES);

	// ###################################################################################################

	// Private functions

	void healthCheck(void);

	//! @brief Will remove the service from the service map aswell as all sessions in this service
	void removeSessionService(LocalSessionService* _service);

	LocalSessionService* leastLoadedSessionService(void);

	GlobalSessionService();
	virtual ~GlobalSessionService();

	// ###################################################################################################

	std::mutex									m_mapMutex;
	std::map<std::string, LocalSessionService*>		m_sessionToServiceMap;
	std::map<ot::serviceID_t, LocalSessionService*>	m_sessionServiceIdMap;
	ot::IDManager<ot::serviceID_t>				m_sessionServiceIdManager;
	bool										m_forceHealthCheck;

	GlobalSessionService(GlobalSessionService&) = delete;
	GlobalSessionService& operator = (GlobalSessionService&) = delete;
};