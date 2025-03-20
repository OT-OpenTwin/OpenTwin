#include "Service.h"
#include "ServiceManager.h"
#include "Configuration.h"
#include "Application.h"

// Open Twin core types header
#include "OTCore/otAssert.h"
#include "OTCore/Logger.h"
#include "OTSystem/SystemProcess.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

// Standard headers
#include <iostream>
#include <chrono>
#include <thread>

Service::Service(ServiceManager * _owner, const ServiceInformation& _info)
	: m_owner(_owner), m_isAlive(false), m_info(_info), m_processHandle(OT_INVALID_PROCESS_HANDLE), m_port(0), m_websocketPort(0), m_startCounter(0)
{
	
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

void Service::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	ot::JsonObject infoObj;
	m_info.addToJsonObject(infoObj, _allocator);

	_object.AddMember("Information", infoObj, _allocator);
	_object.AddMember("StartCounter", m_startCounter, _allocator);
	_object.AddMember("IsAlive", m_isAlive, _allocator);
	_object.AddMember("URL", ot::JsonString(m_url, _allocator), _allocator);
	_object.AddMember("Port", m_port, _allocator);
	_object.AddMember("WebsocketURL", ot::JsonString(m_websocketUrl, _allocator), _allocator);
	_object.AddMember("WebsocketPort", m_websocketPort, _allocator);
}

ot::RunResult Service::run(const SessionInformation& _sessionInformation, const std::string& _url, ot::port_t _port, ot::port_t _websocketPort) {
	OT_LOG_D("Starting service (Name = \"" + m_info.name() + "\"; Type = \"" + m_info.type() + "\"; Session.ID = \"" + 
		_sessionInformation.id() + "\"; LSS.Url = \"" + _sessionInformation.sessionServiceURL() + "\")");

	m_port = _port;
	m_websocketPort = _websocketPort;

	m_url = _url + ":" + std::to_string(m_port);
	m_websocketUrl = _url + ":" + std::to_string(m_websocketPort);

#if defined(OT_OS_WINDOWS)

	m_processHandle = OT_INVALID_PROCESS_HANDLE;

	std::string launcherName = LDS_CFG.launcherPath();

	std::string path = LDS_CFG.servicesLibraryPath();
	if (!path.empty()) path.append("\\");
	path.append(m_info.name()).append(".dll");

	// Websocket: open_twin.exe "libraryName.dll" "LDS URL" "serviceURL" "websocketURL" "sessionServiceURL"
	// Others:    open_twin.exe "libraryName.dll" "LDS URL" "serviceURL" "sessionServiceURL" "session ID"
	std::string commandLine = "\"" + launcherName + "\" \"" + path + "\" \"" + LDS_APP->getServiceURL() + "\" \"" + m_url + "\" \"";
	if (m_info.type() == OT_INFO_SERVICE_TYPE_RelayService) {
		commandLine.append(m_websocketUrl).append("\" \"").append(_sessionInformation.sessionServiceURL()).append("\"");
	}
	else {
		commandLine.append(_sessionInformation.sessionServiceURL()).append("\" \"").append(_sessionInformation.id()).append("\"");
	}
	
	OT_LOG_D("Starting \"" + launcherName + "\" with command line \"" + commandLine + "\"");

	return ot::SystemProcess::runApplication(launcherName, commandLine, m_processHandle);
#else
	otAssert(0, "Not implemented");
	return ot::SystemProcess::GeneralError;
#endif
}

ot::RunResult Service::shutdown(void) {
	
	ot::RunResult result;
	if (m_isAlive) 
	{
	#if defined(OT_OS_WINDOWS)
		const uint32_t somethingWentWrongExitCode = 1;
		bool processTerminated = TerminateProcess(m_processHandle, somethingWentWrongExitCode); //Closing process without condition. Call is asynchronous, on return the process may not be terminated yet!
		if (processTerminated == FALSE)
		{
			result.setAsError(GetLastError());
			result.setErrorMessage("Failed in trying to terminate the process");
		}
		bool handleClosed = CloseHandle(m_processHandle);
		if (!handleClosed)
		{
			result.setAsError(GetLastError());
			result.setErrorMessage("Failed in closing the process handle");
		}
	#endif
		m_isAlive = false;
	}
	m_processHandle = OT_INVALID_PROCESS_HANDLE;
	return result;
}

void Service::incrStartCounter(void) {
	m_startCounter++;
}

ot::RunResult Service::checkAlive() 
{
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
	else 
	{		
		OT_LOG_E("Failed to get exit code");
		result.setAsError(exitCode);
		result.setErrorMessage("Failed to get service exit code { \"Name\": \"" + m_info.name() + "\"; \"Type\": \"" + m_info.type() + "\"; \"URL\": \"" + m_url + "\" }.");
	}

	return result;
#else
	assert(0);
	OT_LOG_E("Function is implemented only for Windows OS");
	return false;
#endif // OT_OS_WINDOWS
}
