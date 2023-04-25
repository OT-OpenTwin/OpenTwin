#pragma once

// OpenTwin header
#include "OpenTwinCore/ServiceBase.h"
#include "OpenTwinCore/Flags.h"
#include "OpenTwinCore/OTClassHelper.h"
#include "OpenTwinCommunication/actionTypes.h"
#include "OpenTwinFoundation/OTObject.h"
#include "OpenTwinFoundation/IDManager.h"

// C++ header
#include <string>
#include <list>
#include <map>
#include <mutex>

class SessionService;

class GlobalSessionService : public ot::OTObject, public ot::ServiceBase {
public:
	static GlobalSessionService * instance(void);
	static bool hasInstance(void);
	static void deleteInstance(void);

	// ###################################################################################################

	// Properties
	OT_PROPERTY(std::string, URL);
	OT_PROPERTY(std::string, DatabaseURL);
	OT_PROPERTY(std::string, AuthorizationServiceURL);
	OT_PROPERTY(std::string, GlobalDirectoryServiceURL);

	// ###################################################################################################

	// Service handling

	bool addSessionService(SessionService& _service);

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
	void removeSessionService(SessionService * _service);

	SessionService * leastLoadedSessionService(void);

	GlobalSessionService();
	virtual ~GlobalSessionService();

	// ###################################################################################################

	// Private properties
	OT_PROPERTY_IS(HealthCheckRunning);

	std::mutex									m_mapMutex;
	std::map<std::string, SessionService *>		m_sessionToServiceMap;
	std::map<ot::serviceID_t, SessionService *>	m_sessionServiceIdMap;
	ot::IDManager<ot::serviceID_t>				m_sessionServiceIdManager;
	bool										m_forceHealthCheck;

	GlobalSessionService(GlobalSessionService&) = delete;
	GlobalSessionService& operator = (GlobalSessionService&) = delete;
};