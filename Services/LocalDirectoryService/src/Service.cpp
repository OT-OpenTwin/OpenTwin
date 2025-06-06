//! @file Service.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "Service.h"
#include "Application.h"
#include "Configuration.h"
#include "ServiceManager.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTSystem/SystemProcess.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// std header
#include <chrono>
#include <thread>

Service::Service(ServiceManager * _owner, const ServiceInformation& _info) :
	m_owner(_owner), m_info(_info), m_processHandle(OT_INVALID_PROCESS_HANDLE), 
	m_port(0), m_websocketPort(0), m_isShuttingDown(false)
{}

Service::Service(Service&& _other) noexcept :
	m_owner(_other.m_owner), m_isShuttingDown(_other.m_isShuttingDown), m_info(std::move(_other.m_info)),
	m_processHandle(_other.m_processHandle), m_port(_other.m_port), m_websocketPort(_other.m_websocketPort),
	m_url(std::move(_other.m_url)), m_websocketUrl(std::move(_other.m_websocketUrl))
{
	_other.m_owner = nullptr;
	_other.m_processHandle = OT_INVALID_PROCESS_HANDLE;
	_other.m_isShuttingDown = false;
}

Service::~Service() {
	if (m_processHandle != OT_INVALID_PROCESS_HANDLE) {
#if defined(OT_OS_WINDOWS)
		CloseHandle(m_processHandle);
#else
		assert(0); // Not implemented
#endif
	}
	m_processHandle = OT_INVALID_PROCESS_HANDLE;
}

Service& Service::operator=(Service&& _other) noexcept {
	if (this != &_other) {
		m_owner = _other.m_owner;
		m_isShuttingDown = _other.m_isShuttingDown;
		m_info = std::move(_other.m_info);
		m_processHandle = _other.m_processHandle;
		m_port = _other.m_port;
		m_websocketPort = _other.m_websocketPort;
		m_url = std::move(_other.m_url);
		m_websocketUrl = std::move(_other.m_websocketUrl);

		_other.m_owner = nullptr;
		_other.m_processHandle = OT_INVALID_PROCESS_HANDLE;
		_other.m_isShuttingDown = false;
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void Service::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	ot::JsonObject infoObj;
	m_info.addToJsonObject(infoObj, _allocator);

	_object.AddMember("Information", infoObj, _allocator);
	_object.AddMember("IsShuttingDown", m_isShuttingDown, _allocator);
	_object.AddMember("URL", ot::JsonString(m_url, _allocator), _allocator);
	_object.AddMember("Port", m_port, _allocator);
	_object.AddMember("WebsocketURL", ot::JsonString(m_websocketUrl, _allocator), _allocator);
	_object.AddMember("WebsocketPort", m_websocketPort, _allocator);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Service control

ot::RunResult Service::run(const std::string& _url, ot::port_t _port, ot::port_t _websocketPort) {
	OT_LOG_D("Starting service { \"Name\": \"" + m_info.getName() + "\", \"Type\": \"" + m_info.getType() + 
		"\", \"Session.ID\": \"" + m_info.getSessionId() + "\", \"LSS.Url\": \"" + m_info.getSessionServiceURL() + "\" }");

	m_port = _port;
	m_url = _url + ":" + std::to_string(m_port);

	if (m_info.getType() == OT_INFO_SERVICE_TYPE_RelayService) {
		m_websocketPort = _websocketPort;
		m_websocketUrl = _url + ":" + std::to_string(m_websocketPort);
	}

#if defined(OT_OS_WINDOWS)

	m_processHandle = OT_INVALID_PROCESS_HANDLE;

	Configuration& cfg = Configuration::instance();
	std::string launcherName = cfg.getLauncherPath();

	std::string path = cfg.getServicesLibraryPath();

	if (!path.empty()) {
		path.append("\\");
	}
	path.append(m_info.getName()).append(".dll");

	std::string commandLine = "\"" + launcherName + "\" \"" + path + "\" \"" +
		Application::instance().getServiceURL() + "\" \"" + m_url + "\" \"";

	if (m_info.getType() == OT_INFO_SERVICE_TYPE_RelayService) {
		// Relay: open_twin.exe "libraryName.dll" "LDS URL" "serviceURL" "websocketURL" "sessionServiceURL"
		commandLine.append(m_websocketUrl).append("\" \"").append(m_info.getSessionServiceURL()).append("\"");
	}
	else {
		// Others: open_twin.exe "libraryName.dll" "LDS URL" "serviceURL" "sessionServiceURL" "session ID"
		commandLine.append(m_info.getSessionServiceURL()).append("\" \"").append(m_info.getSessionId()).append("\"");
	}
	
	OT_LOG_D("Starting \"" + launcherName + "\" with command line \"" + commandLine + "\"");
	
	return ot::SystemProcess::runApplication(launcherName, commandLine, m_processHandle);
#else
	OT_LOG_EA("Not implemented");
	return ot::SystemProcess::GeneralError;
#endif
}

ot::RunResult Service::shutdown(void) {
	ot::RunResult result;

#if defined(OT_OS_WINDOWS)
	const uint32_t somethingWentWrongExitCode = 1;
	bool processTerminated = TerminateProcess(m_processHandle, somethingWentWrongExitCode); // Closing process without condition. Call is asynchronous, on return the process may not be terminated yet!
	if (processTerminated == FALSE) {
		result.setAsError(GetLastError());
		result.setErrorMessage("Failed in trying to terminate the process");
	}
	bool handleClosed = CloseHandle(m_processHandle);
	if (!handleClosed) {
		result.setAsError(GetLastError());
		result.setErrorMessage("Failed in closing the process handle");
	}
#endif
	m_processHandle = OT_INVALID_PROCESS_HANDLE;
	return result;
}

ot::RunResult Service::checkAlive(void) {
	ot::RunResult result;
#if defined(OT_OS_WINDOWS)
	// Checking the exit code of the service
	DWORD exitCode = STILL_ACTIVE;
	if (GetExitCodeProcess(m_processHandle, &exitCode)) {
		if (exitCode != STILL_ACTIVE) {
			result.setAsError(exitCode);
			result.addToErrorMessage("Checked for process state but process is not active anymore.");
		}
	}
	else {		
		OT_LOG_E("Failed to get exit code");
		result.setAsError(exitCode);
		result.setErrorMessage("Failed to get service exit code { \"Name\": \"" + m_info.getName() + "\"; \"Type\": \"" + m_info.getType() + "\"; \"URL\": \"" + m_url + "\" }.");
	}

#else
	OT_LOG_EA("Function is implemented only for Windows OS");
#endif // OT_OS_WINDOWS

	return result;
}
