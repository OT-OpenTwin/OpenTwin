//! @file Application.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// GDS header
#include "StartupDispatcher.h"

// Open twin header
#include "OTSystem/SystemInformation.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/LogModeManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTServiceFoundation/IDManager.h"

// std header
#include <map>
#include <mutex>
#include <string>
#include <vector>

// Forward declarations
class LocalDirectoryService;
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class Application : public ot::ServiceBase {
	OT_DECL_ACTION_HANDLER(Application)
public:
	// ###########################################################################################################################################################################################################################################################################################################################

	// Static methods

	static Application& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Management

	bool requestToRunService(const ServiceInformation& _serviceInfo);

	int initialize(const char* _siteID, const char* _ownURL, const char* _globalSessionServiceURL);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	OT_HANDLER(handleLocalDirectoryServiceConnected, Application, OT_ACTION_CMD_RegisterNewLocalDirecotoryService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleStartService, Application, OT_ACTION_CMD_StartNewService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleStartServices, Application, OT_ACTION_CMD_StartNewServices, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleStartRelayService, Application, OT_ACTION_CMD_StartNewRelayService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceStopped, Application, OT_ACTION_CMD_ServiceDisconnected, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSessionClosing, Application, OT_ACTION_CMD_ShutdownSession, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSessionClosed, Application, OT_ACTION_CMD_ShutdownSessionCompleted, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleUpdateSystemLoad, Application, OT_ACTION_CMD_UpdateSystemLoad, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetSystemInformation, Application, OT_ACTION_CMD_GetSystemInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSetGlobalLogFlags, Application, OT_ACTION_CMD_SetGlobalLogFlags, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetDebugInformation, Application, OT_ACTION_CMD_GetDebugInformation, ot::SECURE_MESSAGE_TYPES)

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private methods

private:
	const std::vector<LocalDirectoryService>& getLocalDirectoryServices(void) const { return m_localDirectoryServices; };

	//! @brief Determines the least loaded directory service that supports the given service.
	//! @warning It is expected that the mutex is locked when calling this method.
	//! @param _info Service information.
	//! @return A pointer to the least loaded directory service that supports the given service, or nullptr if no such service exists.
	LocalDirectoryService* leastLoadedDirectoryService(const ServiceInformation& _info);

	Application();
	virtual ~Application();

	std::string                        m_globalSessionServiceURL;
	std::vector<LocalDirectoryService> m_localDirectoryServices;
	ot::IDManager<ot::serviceID_t>     m_ldsIdManager;
	std::mutex                         m_mutex;
	StartupDispatcher                  m_startupDispatcher;
	ot::SystemInformation              m_systemLoadInformation;
	ot::LogModeManager                 m_logModeManager;
};
