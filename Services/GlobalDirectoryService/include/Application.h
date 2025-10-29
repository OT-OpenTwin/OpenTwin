// @otlicense

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
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTCommunication/ServiceInitData.h"
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

class Application : public ot::ServiceBase, public ot::ActionHandler {
	OT_DECL_ACTION_HANDLER(Application)
public:
	// ###########################################################################################################################################################################################################################################################################################################################

	// Static methods

	static Application& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Management

	bool requestToRunService(const ot::ServiceInitData& _serviceInfo);

	int initialize(const char* _siteID, const char* _ownURL, const char* _globalSessionServiceURL);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	ot::ReturnMessage handleLocalDirectoryServiceConnected(ot::JsonDocument& _jsonDocument);
	ot::ReturnMessage handleStartService(ot::JsonDocument& _jsonDocument);
	ot::ReturnMessage handleStartServices(ot::JsonDocument& _jsonDocument);
	ot::ReturnMessage handleStartRelayService(ot::JsonDocument& _jsonDocument);
	void handleServiceStopped(ot::JsonDocument& _jsonDocument);
	void handleSessionClosing(ot::JsonDocument& _jsonDocument);
	void handleSessionClosed(ot::JsonDocument& _jsonDocument);
	ot::ReturnMessage handleUpdateSystemLoad(ot::JsonDocument& _jsonDocument);
	std::string handleGetSystemInformation();
	void handleSetGlobalLogFlags(ot::JsonDocument& _jsonDocument);
	std::string handleGetDebugInformation();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private methods

private:
	const std::vector<LocalDirectoryService>& getLocalDirectoryServices(void) const { return m_localDirectoryServices; };

	//! @brief Determines the least loaded directory service that supports the given service.
	//! @warning It is expected that the mutex is locked when calling this method.
	//! @param _info Service information.
	//! @return A pointer to the least loaded directory service that supports the given service, or nullptr if no such service exists.
	LocalDirectoryService* leastLoadedDirectoryService(const ot::ServiceInitData& _info);

	bool canStartService(const ot::ServiceInitData& _info) const;

	Application();
	virtual ~Application();

	std::string                        m_globalSessionServiceURL;
	std::vector<LocalDirectoryService> m_localDirectoryServices;
	ot::IDManager<ot::serviceID_t>     m_ldsIdManager;
	std::mutex                         m_mutex;
	StartupDispatcher                  m_startupDispatcher;
	ot::SystemInformation              m_systemLoadInformation;
};
