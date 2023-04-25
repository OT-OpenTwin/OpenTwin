#include "GlobalDirectoryService.h"
#include "Application.h"

#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinSystem/OperatingSystem.h"
#include "OpenTwinCommunication/Msg.h"
#include "OpenTwinCommunication/ActionTypes.h"

#include <iostream>
#include <thread>

GlobalDirectoryService::GlobalDirectoryService(void)
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, OT_INFO_SERVICE_TYPE_GlobalDirectoryService, "", ot::invalidServiceID, "0"),
	m_connectionStatus(Disconnected)
{
	
}

GlobalDirectoryService::~GlobalDirectoryService() {

}

// #################################################################################################################################

// Connection

bool GlobalDirectoryService::isConnected(void) const {
	return m_connectionStatus == Connected;
}

void GlobalDirectoryService::connect(const std::string& _url) {
	setServiceURL(_url);
	m_connectionStatus = WaitingForConnection;

	std::thread t(&GlobalDirectoryService::registerAtGlobalDirectoryService, this);
	t.detach();
}

// #################################################################################################################################

// Private functions

void GlobalDirectoryService::registerAtGlobalDirectoryService(void) {
	
	OT_rJSON_createDOC(registerDoc);
	ot::rJSON::add(registerDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_RegisterNewLocalDirecotoryService);
	ot::rJSON::add(registerDoc, OT_ACTION_PARAM_SERVICE_URL, LDS_APP->serviceURL());
	ot::rJSON::add(registerDoc, OT_ACTION_PARAM_SUPPORTED_SERVICES, LDS_APP->supportedServices());
	addSystemValues(registerDoc);

	// Send request and check if the request was successful
	std::string response;
	if (!ot::msg::send(LDS_APP->serviceURL(), m_serviceURL, ot::EXECUTE, ot::rJSON::toJSON(registerDoc), response)) {
		OT_LOG_E("Failed to send register request to global directory service");
		return;
	}
	if (response.find(OT_ACTION_RETURN_INDICATOR_Error) != std::string::npos ||
		response.find(OT_ACTION_RETURN_INDICATOR_Warning) != std::string::npos) {
		OT_LOG_E("Register at GDS failed:" + response);
		return;
	}
	if (response == OT_ACTION_RETURN_VALUE_FAILED) {
		OT_LOG_E("Registration at global directory service failed");
		return;
	}

	// Check response
	OT_rJSON_parseDOC(responseDoc, response.c_str());
	if (!responseDoc.IsObject()) { OT_LOG_E("Register at GDS, invalid response"); return; }
	if (!responseDoc.HasMember(OT_ACTION_PARAM_SERVICE_ID)) { OT_LOG_E("Register at GDS, invalid response: Missing member"); return; }
	if (!responseDoc[OT_ACTION_PARAM_SERVICE_ID].IsUint()) { OT_LOG_E("Register at GDS, invalid response: Invalid member type"); return; }

	LDS_APP->setServiceID(responseDoc[OT_ACTION_PARAM_SERVICE_ID].GetUint());

	OT_LOG_I("Registration at Global Directory Service successful");
	m_connectionStatus = Connected;
	healthCheck();
}

void GlobalDirectoryService::healthCheck(void) {
	OT_LOG_I("Starting Global Directory Service health check");
	while (!m_isShuttingDown) {

		OT_rJSON_createDOC(systemStatusDoc);
		ot::rJSON::add(systemStatusDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UpdateSystemLoad);
		ot::rJSON::add(systemStatusDoc, OT_ACTION_PARAM_SERVICE_ID, LDS_APP->serviceID());
		addSystemValues(systemStatusDoc);

		std::string response;
		if (!ot::msg::send(LDS_APP->serviceURL(), m_serviceURL, ot::EXECUTE, ot::rJSON::toJSON(systemStatusDoc), response)) {
			OT_LOG_E("Failed to send updated system load to global directory service");
			LDS_APP->globalDirectoryServiceCrashed();
			return;
		}

		if (response != OT_ACTION_RETURN_VALUE_OK) {
			OT_LOG_E("Health check for GDS: " + response);
			LDS_APP->globalDirectoryServiceCrashed();
			return;
		}

		int ct = 0;
		while (ct < 60 && !m_isShuttingDown) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
	}
}

void GlobalDirectoryService::addSystemValues(OT_rJSON_doc& _jsonDocument) {
	ot::rJSON::add(_jsonDocument, OT_ACTION_PARAM_SYSTEM_AvailablePhysicalMemory, std::to_string(ot::os::getAvailablePhysicalMemory()));
	ot::rJSON::add(_jsonDocument, OT_ACTION_PARAM_SYSTEM_AvailableVirtualMemory, std::to_string(ot::os::getAvailableVirtualMemory()));
	ot::rJSON::add(_jsonDocument, OT_ACTION_PARAM_SYSTEM_TotalPhysicalMemory, std::to_string(ot::os::getTotalPhysicalMemory()));
	ot::rJSON::add(_jsonDocument, OT_ACTION_PARAM_SYSTEM_TotalVirtualMemory, std::to_string(ot::os::getTotalVirtualMemory()));
}