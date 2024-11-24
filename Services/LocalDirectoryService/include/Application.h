/*
 * Application.h
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

#pragma once

#include "GlobalDirectoryService.h"
#include "ServiceManager.h"

// Open twin header
#include "OTCore/ServiceBase.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTCommunication/ActionHandleConnector.h"
#include "OTSystem/SystemInformation.h"

// C++ header
#include <string>
#include <list>

// Forward declaration
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

//! Short form to get the Application instance
#define LDS_APP Application::instance()

class Application : public ot::ServiceBase, public ot::ActionHandler {
public:
	static Application * instance(void);
	static void deleteInstance(void);

private:
	Application();
	virtual ~Application();

	// ##################################################################################################################################

	// Private functions

	OT_HANDLER(handleStartNewService, Application, OT_ACTION_CMD_StartNewService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleStartNewRelayService, Application, OT_ACTION_CMD_StartNewRelayService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSessionClosed, Application, OT_ACTION_CMD_ShutdownSessionCompleted, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceClosed, Application, OT_ACTION_CMD_ServiceDisconnected, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetDebugInformation, Application, OT_ACTION_CMD_GetDebugInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetSystemInformation, Application, OT_ACTION_CMD_GetSystemInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSetGlobalLogFlags, Application, OT_ACTION_CMD_SetGlobalLogFlags, ot::SECURE_MESSAGE_TYPES)

	ServiceManager				m_serviceManager;
	GlobalDirectoryService		m_globalDirectoryService;
	ot::SystemInformation		m_systemLoadInformation;

	// ##################################################################################################################################
public:
	// Public functions

	int initialize(const char * _ownURL, const char * _globalDirectoryServiceURL);

	void globalDirectoryServiceCrashed(void);
	
	ServiceManager& serviceManager(void) { return m_serviceManager; }
	std::list<std::string> supportedServices(void) const;
};
