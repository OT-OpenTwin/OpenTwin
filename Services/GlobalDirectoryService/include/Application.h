/*
 * Application.h
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

#pragma once

#include "StartupDispatcher.h"

// Open twin header
#include <OpenTwinCore/ServiceBase.h>		// Base class
#include <OpenTwinFoundation/OTObject.h>		// Base class
#include <OpenTwinCore/rJSON.h>					// open twins rapid json layer
#include <OpenTwinCommunication/actionTypes.h>
#include <OpenTwinFoundation/IDManager.h>

// C++ header
#include <string>
#include <vector>
#include <map>
#include <mutex>

// Forward declaration
class LocalDirectoryService;
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class Application : public ot::ServiceBase, public ot::OTObject {
public:
	static Application * instance(void);
	static void deleteInstance(void);

private:
	Application();
	virtual ~Application();

	std::string								m_globalSessionServiceURL;
	std::vector<LocalDirectoryService *>	m_localDirectoryServices;
	ot::IDManager<ot::serviceID_t>			m_ldsIdManager;
	std::mutex								m_mutex;
	StartupDispatcher						m_startupDispatcher;

public:

	const std::vector<LocalDirectoryService *>& localDirectoryServices(void) const { return m_localDirectoryServices; }
	LocalDirectoryService * leastLoadedDirectoryService(const ServiceStartupInformation& _info);

	OT_HANDLER(handleLocalDirectoryServiceConnected, Application, OT_ACTION_CMD_RegisterNewLocalDirecotoryService, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleStartService, Application, OT_ACTION_CMD_StartNewService, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleStartServices, Application, OT_ACTION_CMD_StartNewServices, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleStartRelayService, Application, OT_ACTION_CMD_StartNewRelayService, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleServiceStopped, Application, OT_ACTION_CMD_ServiceDisconnected, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleSessionClosed, Application, OT_ACTION_CMD_ShutdownSessionCompleted, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleUpdateSystemLoad, Application, OT_ACTION_CMD_UpdateSystemLoad, ot::SECURE_MESSAGE_TYPES);

	bool requestToRunService(const ServiceStartupInformation& _info);

	int initialize(const char * _siteID, const char * _ownURL, const char * _globalSessionServiceURL);
};
