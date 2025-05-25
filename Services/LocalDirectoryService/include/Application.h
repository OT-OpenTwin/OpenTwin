//! @file Application.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LDS header
#include "ServiceManager.h"
#include "GlobalDirectoryService.h"

// OpenTwin header
#include "OTSystem/SystemInformation.h"
#include "OTCore/ServiceBase.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"

// std header
#include <list>
#include <string>

class Application : public ot::ServiceBase {
	OT_DECL_ACTION_HANDLER(Application)
public:
	static Application& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public functions

	int initialize(const char * _ownURL, const char * _globalDirectoryServiceURL);

	//! @brief Handle the crash of the global directory service.
	//! @warning The crash of the GDS is not handled yet. The application will exit with the GDSNotRunning error code.
	void globalDirectoryServiceCrashed(void);
	
	ServiceManager& getServiceManager(void) { return m_serviceManager; }

	std::list<std::string> getSupportedServices(void) const;

private:

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	OT_HANDLER(handleStartNewService, Application, OT_ACTION_CMD_StartNewService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleStartNewRelayService, Application, OT_ACTION_CMD_StartNewRelayService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSessionClosed, Application, OT_ACTION_CMD_ShutdownSessionCompleted, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceClosed, Application, OT_ACTION_CMD_ServiceDisconnected, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetDebugInformation, Application, OT_ACTION_CMD_GetDebugInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetSystemInformation, Application, OT_ACTION_CMD_GetSystemInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSetGlobalLogFlags, Application, OT_ACTION_CMD_SetGlobalLogFlags, ot::SECURE_MESSAGE_TYPES)

	// ###########################################################################################################################################################################################################################################################################################################################

	// Constructor/Destructor

	Application();
	virtual ~Application();

	ServiceManager				m_serviceManager;
	GlobalDirectoryService		m_globalDirectoryService;
	ot::SystemInformation		m_systemLoadInformation;
};
