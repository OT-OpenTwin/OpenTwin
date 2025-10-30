// @otlicense

// LDS header
#include "Application.h"
#include "GlobalDirectoryService.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/LogDispatcher.h"
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

// ###########################################################################################################################################################################################################################################################################################################################

// Connection

void GlobalDirectoryService::connect(const std::string& _url) {
	this->setServiceURL(_url);

	m_connectionStatus = WaitingForConnection;

	std::thread t(&GlobalDirectoryService::registerAtGlobalDirectoryService, this);
	t.detach();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Worker functions

void GlobalDirectoryService::registerAtGlobalDirectoryService(void) {
	Application& app = Application::instance();

	ot::JsonDocument registerDoc;
	registerDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewLocalDirecotoryService, registerDoc.GetAllocator()), registerDoc.GetAllocator());
	registerDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(app.getServiceURL(), registerDoc.GetAllocator()), registerDoc.GetAllocator());
	registerDoc.AddMember(OT_ACTION_PARAM_SUPPORTED_SERVICES, ot::JsonArray(app.getSupportedServices(), registerDoc.GetAllocator()), registerDoc.GetAllocator());
	
	this->addSystemValues(registerDoc);

	// Send request and check if the request was successful
	std::string responseStr;

	// In case of error:
	// Minimum timeout: attempts * thread sleep                  = 30 * 500ms        =   15sec
	// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
	const int maxCt = 30;
	int ct = 1;
	bool ok = false;
	do {
		responseStr.clear();
		if (!(ok = ot::msg::send(app.getServiceURL(), this->getServiceURL(), ot::EXECUTE, registerDoc.toJson(), responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
			OT_LOG_E("Register at Global Directory Service (" + Application::instance().getServiceURL() + ") failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(500ms);
		}
	} while (!ok && ct++ <= maxCt);

	if (!ok) {
		OT_LOG_E("Registration at Global Directory Service (" + Application::instance().getServiceURL() + ") failed after " + std::to_string(maxCt) + " attempts. Exiting...");
		exit(ot::AppExitCode::GDSRegistrationFailed);
	}

	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
	if (!response.isOk()) {
		OT_LOG_E("Register at GDS failed: " + response.getWhat());
		return;
	}

	// Check response
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response.getWhat());
	if (!responseDoc.IsObject()) { OT_LOG_E("Register at GDS, invalid response"); return; }
	if (!responseDoc.HasMember(OT_ACTION_PARAM_SERVICE_ID)) { OT_LOG_E("Register at GDS, invalid response: Missing member"); return; }
	if (!responseDoc[OT_ACTION_PARAM_SERVICE_ID].IsUint()) { OT_LOG_E("Register at GDS, invalid response: Invalid member type"); return; }

	app.setServiceID(responseDoc[OT_ACTION_PARAM_SERVICE_ID].GetUint());

	if (responseDoc.HasMember(OT_ACTION_PARAM_GlobalLogFlags)) {
		ot::ConstJsonArray logFlags = ot::json::getArray(responseDoc, OT_ACTION_PARAM_GlobalLogFlags);
		ot::LogDispatcher::instance().setLogFlags(ot::logFlagsFromJsonArray(logFlags));
	}

	OT_LOG_I("Registration at Global Directory Service successful");
	m_connectionStatus = Connected;
	
	this->healthCheck();
}

void GlobalDirectoryService::healthCheck(void) {
	OT_LOG_I("Starting Global Directory Service health check");

	Application& app = Application::instance();

	while (!m_isShuttingDown) {
		ot::JsonDocument systemStatusDoc;
		systemStatusDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UpdateSystemLoad, systemStatusDoc.GetAllocator()), systemStatusDoc.GetAllocator());
		systemStatusDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, app.getServiceID(), systemStatusDoc.GetAllocator());
		
		addSystemValues(systemStatusDoc);

		std::string responseStr;
		if (!ot::msg::send(app.getServiceURL(), this->getServiceURL(), ot::EXECUTE, systemStatusDoc.toJson(), responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			OT_LOG_E("Failed to send updated system load to global directory service");
			app.globalDirectoryServiceCrashed();
			break;
		}

		ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);

		if (!response.isOk()) {
			OT_LOG_E("Health check for GDS failed: " + response.getWhat());
			app.globalDirectoryServiceCrashed();
			break;
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
