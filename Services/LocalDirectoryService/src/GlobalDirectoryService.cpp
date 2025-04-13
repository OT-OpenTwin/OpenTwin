// Service header
#include "Application.h"
#include "GlobalDirectoryService.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// std header
#include <thread>
#include <chrono>

GlobalDirectoryService::GlobalDirectoryService(void)
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, OT_INFO_SERVICE_TYPE_GlobalDirectoryService, "", ot::invalidServiceID, "0"),
	m_connectionStatus(Disconnected), m_isShuttingDown(false)
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
	
	ot::JsonDocument registerDoc;
	registerDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewLocalDirecotoryService, registerDoc.GetAllocator()), registerDoc.GetAllocator());
	registerDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(LDS_APP->getServiceURL(), registerDoc.GetAllocator()), registerDoc.GetAllocator());
	registerDoc.AddMember(OT_ACTION_PARAM_SUPPORTED_SERVICES, ot::JsonArray(LDS_APP->supportedServices(), registerDoc.GetAllocator()), registerDoc.GetAllocator());
	addSystemValues(registerDoc);

	// Send request and check if the request was successful
	std::string response;

	// In case of error:
	// Minimum timeout: attempts * thread sleep                  = 30 * 500ms        =   15sec
	// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;
	do {
		response.clear();
		if (!(ok = ot::msg::send(LDS_APP->getServiceURL(), m_serviceURL, ot::EXECUTE, registerDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			OT_LOG_E("Register at Global Directory Service (" + Application::instance()->getServiceURL() + ") failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);
		}
	} while (!ok && ct++ <= maxCt);

	if (!ok) {
		OT_LOG_E("Registration at Global Directory Service (" + Application::instance()->getServiceURL() + ") failed after " + std::to_string(maxCt) + " attempts. Exiting...");
		exit(ot::AppExitCode::GDSRegistrationFailed);
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
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	if (!responseDoc.IsObject()) { OT_LOG_E("Register at GDS, invalid response"); return; }
	if (!responseDoc.HasMember(OT_ACTION_PARAM_SERVICE_ID)) { OT_LOG_E("Register at GDS, invalid response: Missing member"); return; }
	if (!responseDoc[OT_ACTION_PARAM_SERVICE_ID].IsUint()) { OT_LOG_E("Register at GDS, invalid response: Invalid member type"); return; }

	LDS_APP->setServiceID(responseDoc[OT_ACTION_PARAM_SERVICE_ID].GetUint());

	if (responseDoc.HasMember(OT_ACTION_PARAM_GlobalLogFlags)) {
		ot::ConstJsonArray logFlags = ot::json::getArray(responseDoc, OT_ACTION_PARAM_GlobalLogFlags);
		ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(logFlags));
	}

	OT_LOG_I("Registration at Global Directory Service successful");
	m_connectionStatus = Connected;
	healthCheck();
}

void GlobalDirectoryService::healthCheck(void) {
	OT_LOG_I("Starting Global Directory Service health check");
	while (!m_isShuttingDown) {

		ot::JsonDocument systemStatusDoc;
		systemStatusDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UpdateSystemLoad, systemStatusDoc.GetAllocator()), systemStatusDoc.GetAllocator());
		systemStatusDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, LDS_APP->getServiceID(), systemStatusDoc.GetAllocator());
		
		addSystemValues(systemStatusDoc);

		std::string response;
		if (!ot::msg::send(LDS_APP->getServiceURL(), m_serviceURL, ot::EXECUTE, systemStatusDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
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

void GlobalDirectoryService::addSystemValues(ot::JsonDocument& _jsonDocument) {
	_jsonDocument.AddMember(OT_ACTION_PARAM_SYSTEM_AvailablePhysicalMemory, ot::JsonString(std::to_string(ot::OperatingSystem::getAvailablePhysicalMemory()), _jsonDocument.GetAllocator()), _jsonDocument.GetAllocator());
	_jsonDocument.AddMember(OT_ACTION_PARAM_SYSTEM_AvailableVirtualMemory, ot::JsonString(std::to_string(ot::OperatingSystem::getAvailableVirtualMemory()), _jsonDocument.GetAllocator()), _jsonDocument.GetAllocator());
	_jsonDocument.AddMember(OT_ACTION_PARAM_SYSTEM_TotalPhysicalMemory, ot::JsonString(std::to_string(ot::OperatingSystem::getTotalPhysicalMemory()), _jsonDocument.GetAllocator()), _jsonDocument.GetAllocator());
	_jsonDocument.AddMember(OT_ACTION_PARAM_SYSTEM_TotalVirtualMemory, ot::JsonString(std::to_string(ot::OperatingSystem::getTotalVirtualMemory()), _jsonDocument.GetAllocator()), _jsonDocument.GetAllocator());
}
