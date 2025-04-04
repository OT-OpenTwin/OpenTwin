//! @file GlobalSessionService.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

#define NOMINMAX //Supress windows macros min max
#define WIN32_LEAN_AND_MEAN

// GSS header
#include "GlobalSessionService.h"
#include "LocalSessionService.h"
#include "Session.h"

// OpenTwin header
#include "OTSystem/SystemInformation.h"
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ServiceLogNotifier.h"
#include "DataBase.h"

#include "DocumentAPI.h"
#include "Connection\ConnectionAPI.h"
#include "Document\DocumentAccess.h"

// Third party header
#include "base64.h"
#include "zlib.h"

// std header
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <limits>
#include <filesystem>

GlobalSessionService * g_instance{ nullptr };

GlobalSessionService * GlobalSessionService::instance(void) {
	if (g_instance == nullptr) g_instance = new GlobalSessionService;
	return g_instance;
}

bool GlobalSessionService::hasInstance(void) {
	return g_instance != nullptr;
}

void GlobalSessionService::deleteInstance(void) {
	if (g_instance) delete g_instance;
	g_instance = nullptr;
}

// ###################################################################################################

// Service handling

bool GlobalSessionService::addSessionService(LocalSessionService& _service) {
	std::lock_guard<std::mutex> lock(m_mapMutex);

	// Check URL duplicate
	for (auto it : m_sessionServiceIdMap) {
		if (it.second->url() == _service.url()) {
			OT_LOG_W("Session with the url (" + _service.url() + ") is already registered");
			OTAssert(0, "Session url already registered");
			_service.setId(it.second->id());
			return true;
		}
	}

	// Create copy and assign uid
	LocalSessionService* newEntry = new LocalSessionService(_service);
	ot::serviceID_t newID = m_sessionServiceIdManager.grabNextID();
	_service.setId(newID);
	newEntry->setId(newID);

	OT_LOG_D("New session service registered @ " + newEntry->url() + " with id=" + std::to_string(newEntry->id()));

	// Register already opened sessions
	for (auto session : newEntry->sessions()) {
		m_sessionToServiceMap.insert_or_assign(session->id(), newEntry);
		OT_LOG_D("Session service contains already running session with id=" + session->id());
	}
	m_sessionServiceIdMap.insert_or_assign(newID, newEntry);

	// Check if the health check is aready running, otherwise start it
	if (!m_healthCheckRunning) {
		m_healthCheckRunning = true;
		std::thread worker(&GlobalSessionService::healthCheck, this);
		worker.detach();
	}

	return true;
}

void GlobalSessionService::setDatabaseUrl(const std::string& _url)
{
	 m_databaseUrl = _url;
	 if (m_databaseUrl.find("tls@") != std::string::npos)
	 {
		 m_databaseUrl = m_databaseUrl.substr(4);
	 }
}

// ###################################################################################################

// Handler

std::string GlobalSessionService::handleGetDBUrl(ot::JsonDocument& _doc) {
	return m_databaseUrl;
}

std::string GlobalSessionService::handleGetGlobalServicesURL(ot::JsonDocument& _doc) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_PARAM_SERVICE_DBURL, ot::JsonString(m_databaseUrl, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_AUTHURL, ot::JsonString(m_authorizationUrl, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_GDSURL, ot::JsonString(m_globalDirectoryUrl, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(ot::LogDispatcher::instance().logFlags(), flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GlobalLogFlags, flagsArr, doc.GetAllocator());

	return doc.toJson();
}

std::string GlobalSessionService::handleCreateSession(ot::JsonDocument& _doc) {
	Session newSession;
	newSession.setId(ot::json::getString(_doc, OT_ACTION_PARAM_SESSION_ID));
	newSession.setUserName(ot::json::getString(_doc, OT_ACTION_PARAM_USER_NAME));

	std::lock_guard<std::mutex> lock(m_mapMutex);

	// Check if the session already exists
	auto it = m_sessionToServiceMap.find(newSession.id());
	if (it != m_sessionToServiceMap.end()) {
		const Session* session = it->second->getSessionById(newSession.id());
		OTAssertNullptr(session);

		// Check if the username differs
		if (session->userName() != newSession.userName()) {
			OT_LOG_WAS("Session opened by other user");
			ot::ReturnMessage response(ot::ReturnMessage::Failed, "Session open by other user");
			return response.toJson();
		}
		else {
			ot::ReturnMessage response(ot::ReturnMessage::Failed, "Session already open in another frontend instance or from another device.");
			return response.toJson();
		}
	}
	else {
		LocalSessionService* ss = leastLoadedSessionService();
		if (ss) {
			// Register set session as open for the specified LSS.
			ss->addSession(newSession);

			// This may be only problematic if the user/service requesting to create a new session will crash after receiving
			// the response and won't continue with the create session request to the provided LSS.
			m_sessionToServiceMap.insert_or_assign(newSession.id(), ss);

			OT_LOG_D("Session service @ " + ss->url() + " with id=" + std::to_string(ss->id()) + " provided for session with id=" + newSession.id());
			ot::ReturnMessage response(ot::ReturnMessage::Ok, ss->url());
			return response.toJson();
		}
		else {
			OT_LOG_E("No session service connected");
			OTAssert(0, "No session service connected");
			ot::ReturnMessage response(ot::ReturnMessage::Failed, "No session service connected");
			return response.toJson();
		}
	}
}

std::string GlobalSessionService::handleCheckProjectOpen(ot::JsonDocument& _doc) {
	std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);

	std::lock_guard<std::mutex> lock(m_mapMutex);

	//todo: replace response by a document
	auto it = m_sessionToServiceMap.find(projectName);
	if (it == m_sessionToServiceMap.end()) {
		return std::string();
	}
	else {
		Session* session = it->second->getSessionById(projectName);
		if (session) {
			return session->userName();
		}
		else {
			OT_LOG_E("Session \"" + projectName + "\" does not exist in the LSS at \"" + it->second->url() + "\"");
			return std::string("<error while searching for user>");
		}
	}
}

std::string GlobalSessionService::handleGetProjectTemplatesList(ot::JsonDocument& _doc) {
	std::string user = ot::json::getString(_doc, OT_PARAM_DB_USERNAME);
	std::string password = ot::json::getString(_doc, OT_PARAM_DB_PASSWORD);

	ot::JsonDocument result(rapidjson::kArrayType);

	// Get custom templates
	this->getCustomProjectTemplates(result, user, password);

	// Add default templates
	{
		using namespace ot;
		ProjectTemplateInformation default3D;
		default3D.setName(OT_ACTION_PARAM_SESSIONTYPE_3DSIM);
		default3D.setProjectType(OT_ACTION_PARAM_SESSIONTYPE_3DSIM);
		default3D.setIsDefault(true);

		StyledTextBuilder description;
		description << StyledText::Header1 << "3D Simulation project" << StyledText::Text <<
			"Create, import, export and modify 3D geometries. Run simulations.";

		default3D.setDescription(description);

		JsonObject obj3D;
		default3D.addToJsonObject(obj3D, result.GetAllocator());
		result.PushBack(obj3D, result.GetAllocator());
	}

	{
		using namespace ot;
		ProjectTemplateInformation defaultPipeline;
		defaultPipeline.setName(OT_ACTION_PARAM_SESSIONTYPE_DATAPIPELINE);
		defaultPipeline.setProjectType(OT_ACTION_PARAM_SESSIONTYPE_DATAPIPELINE);
		defaultPipeline.setIsDefault(true);

		StyledTextBuilder description;
		description << StyledText::Header1 << "Data pipeline project" << StyledText::Text <<
			"Create automations.";

		defaultPipeline.setDescription(description);

		JsonObject objPipeline;
		defaultPipeline.addToJsonObject(objPipeline, result.GetAllocator());
		result.PushBack(objPipeline, result.GetAllocator());
	}

	{
		using namespace ot;
		ProjectTemplateInformation defaultCST;
		defaultCST.setName(OT_ACTION_PARAM_SESSIONTYPE_STUDIOSUITE);
		defaultCST.setProjectType(OT_ACTION_PARAM_SESSIONTYPE_STUDIOSUITE);
		defaultCST.setIsDefault(true);

		StyledTextBuilder description;
		description << StyledText::Header1 << "CST Studio Suite Project" << StyledText::Text <<
			"Import and export projects from CST Studio Suite.";

		defaultCST.setDescription(description);

		JsonObject objCST;
		defaultCST.addToJsonObject(objCST, result.GetAllocator());
		result.PushBack(objCST, result.GetAllocator());
	}

	{
		using namespace ot;
		ProjectTemplateInformation defaultLTSpice;
		defaultLTSpice.setName(OT_ACTION_PARAM_SESSIONTYPE_LTSPICE);
		defaultLTSpice.setProjectType(OT_ACTION_PARAM_SESSIONTYPE_LTSPICE);
		defaultLTSpice.setIsDefault(true);

		StyledTextBuilder description;
		description << StyledText::Header1 << "LT Spice Project" << StyledText::Text <<
			"Import and export projects from LT Spice.";

		defaultLTSpice.setDescription(description);

		ot::JsonObject objLTSpice;
		defaultLTSpice.addToJsonObject(objLTSpice, result.GetAllocator());
		result.PushBack(objLTSpice, result.GetAllocator());
	}

	{
		using namespace ot;
		ProjectTemplateInformation defaultPyrit;
		defaultPyrit.setName(OT_ACTION_PARAM_SESSIONTYPE_PYRIT);
		defaultPyrit.setProjectType(OT_ACTION_PARAM_SESSIONTYPE_PYRIT);
		defaultPyrit.setIsDefault(true);

		StyledTextBuilder description;
		description << StyledText::Header1 << "Pyrit Project" << StyledText::Text <<
			"Create, import, export and modify 3D geometries. Run simulations using the Pyrit solver.";

		defaultPyrit.setDescription(description);

		ot::JsonObject objPyrit;
		defaultPyrit.addToJsonObject(objPyrit, result.GetAllocator());
		result.PushBack(objPyrit, result.GetAllocator());
	}

	{
		using namespace ot;
		ProjectTemplateInformation defaultCircuitSimulation;
		defaultCircuitSimulation.setName(OT_ACTION_PARAM_SESSIONTYPE_CIRCUITSIMULATION);
		defaultCircuitSimulation.setProjectType(OT_ACTION_PARAM_SESSIONTYPE_CIRCUITSIMULATION);
		defaultCircuitSimulation.setIsDefault(true);

		StyledTextBuilder description;
		description << StyledText::Header1 << "Circuit Simulation Project" << StyledText::Text <<
			"Create and modify electrical circuits. Run circuit simulations.";

		defaultCircuitSimulation.setDescription(description);

		ot::JsonObject objPyrit;
		defaultCircuitSimulation.addToJsonObject(objPyrit, result.GetAllocator());
		result.PushBack(objPyrit, result.GetAllocator());
	}

	{
		using namespace ot;
		ProjectTemplateInformation defaultDevelopment;
		defaultDevelopment.setName(OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT);
		defaultDevelopment.setProjectType(OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT);
		defaultDevelopment.setBriefDescription("All OpenTwin services");
		defaultDevelopment.setIsDefault(true);

		StyledTextBuilder description;
		description << StyledText::Header1 << "Development Project" << StyledText::Text <<
			"Run all services provided by OpenTwin in one session.";

		defaultDevelopment.setDescription(description);

		JsonObject objDev;
		defaultDevelopment.addToJsonObject(objDev, result.GetAllocator());
		result.PushBack(objDev, result.GetAllocator());
	}

	return result.toJson();
}

std::string GlobalSessionService::handleGetBuildInformation(ot::JsonDocument& _doc)
{
	ot::SystemInformation info;
	std::string buildInfo = info.getBuildInformation();

	return buildInfo;
}

std::string GlobalSessionService::handlePrepareFrontendInstaller(ot::JsonDocument& _doc)
{
	m_frontendInstallerFileContent.clear();
	
	loadFrontendInstallerFile();

	return std::to_string(m_frontendInstallerFileContent.size()+1);
}

std::string GlobalSessionService::handleGetFrontendInstaller(ot::JsonDocument& _doc)
{
	if (m_frontendInstallerFileContent.empty())
	{
		loadFrontendInstallerFile();
	}

	std::string tmp = m_frontendInstallerFileContent;

	m_frontendInstallerFileContent.clear();

	return tmp;
}

void GlobalSessionService::loadFrontendInstallerFile()
{
	std::string installerPath;

#ifdef OT_OS_WINDOWS
	char path[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, path, MAX_PATH);
	installerPath = path;
#else
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	installerPath = std::string(result, (count > 0) ? count : 0);
#endif

	installerPath = installerPath.substr(0, installerPath.rfind('\\'));
	installerPath += "\\FrontendInstaller\\Install_OpenTwin_Frontend.exe";

	if (!std::filesystem::exists(installerPath)) {
		// Get the development root environment variable and build the path to the deployment cert file
		char buffer[4096];
		size_t environmentVariableValueStringLength;
		getenv_s(&environmentVariableValueStringLength, buffer, sizeof(buffer) - 1, "OPENTWIN_DEV_ROOT");

		std::string dev_root(buffer);
		installerPath = dev_root + "\\Deployment\\FrontendInstaller\\Install_OpenTwin_Frontend.exe";
	}

	readFileContent(installerPath, m_frontendInstallerFileContent);
}

void GlobalSessionService::readFileContent(const std::string& fileName, std::string& fileContent)
{
	fileContent.clear();

	// Read the file content
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	int data_length = (int)file.tellg();
	file.seekg(0, std::ios::beg);

	char* data = new char[data_length];
	if (!file.read(data, data_length)) return;

	// Convert the binary to an encoded string
	int encoded_data_length = Base64encode_len(data_length);
	char* base64_string = new char[encoded_data_length];

	Base64encode(base64_string, data, data_length); // "base64_string" is a then null terminated string that is an encoding of the binary data pointed to by "data"

	fileContent = std::string(base64_string);

	delete[] base64_string;
	base64_string = nullptr;

	delete[] data;
	data = nullptr;
}


std::string GlobalSessionService::handleGetSystemInformation(ot::JsonDocument& _doc) {

	double globalCpuLoad = 0, globalMemoryLoad = 0;
	m_SystemLoadInformation.getGlobalCPUAndMemoryLoad(globalCpuLoad, globalMemoryLoad);

	double processCpuLoad = 0, processMemoryLoad = 0;
	m_SystemLoadInformation.getCurrentProcessCPUAndMemoryLoad(processCpuLoad, processMemoryLoad);

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_CPU_LOAD, globalCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_MEMORY_LOAD, globalMemoryLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_CPU_LOAD, processCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_MEMORY_LOAD, processMemoryLoad, reply.GetAllocator());

	// Now we add information about the session services
	ot::JsonArray servicesInfo;

	for (auto it : m_sessionServiceIdMap) {
		ot::JsonValue info;
		info.SetObject();

		info.AddMember(OT_ACTION_PARAM_LSS_URL, ot::JsonString(it.second->url(), reply.GetAllocator()), reply.GetAllocator());

		servicesInfo.PushBack(info, reply.GetAllocator());
	}

	reply.AddMember(OT_ACTION_PARAM_LSS, servicesInfo, reply.GetAllocator());

	return reply.toJson();
}

std::string GlobalSessionService::handleRegisterSessionService(ot::JsonDocument& _doc) {
	LocalSessionService nService;
	// Gather information from document
	nService.setFromJsonObject(_doc.GetConstObject());

	// Add to GSS
	if (!addSessionService(nService)) { return OT_ACTION_RETURN_INDICATOR_Error "Failed to attach service information"; }

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_SERVICE_ID, nService.id(), reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(m_databaseUrl, reply.GetAllocator()), reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_SERVICE_AUTHURL, ot::JsonString(m_authorizationUrl, reply.GetAllocator()), reply.GetAllocator());
	if (!m_globalDirectoryUrl.empty()) {
		reply.AddMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL, ot::JsonString(m_globalDirectoryUrl, reply.GetAllocator()), reply.GetAllocator());
	}

	if (m_logModeManager.getGlobalLogFlagsSet()) {
		ot::JsonArray flagsArr;
		ot::addLogFlagsToJsonArray(m_logModeManager.getGlobalLogFlags(), flagsArr, reply.GetAllocator());
		reply.AddMember(OT_ACTION_PARAM_GlobalLogFlags, flagsArr, reply.GetAllocator());
	}

	return reply.toJson();
}

std::string GlobalSessionService::handleShutdownSession(ot::JsonDocument& _doc) {
	std::string sessionID = ot::json::getString(_doc, OT_ACTION_PARAM_SESSION_ID);

	std::lock_guard<std::mutex> lock(m_mapMutex);

	auto it = m_sessionToServiceMap.find(sessionID);
	if (it != m_sessionToServiceMap.end()) {
		LocalSessionService* ss = it->second;
		ss->removeSession(ss->getSessionById(sessionID));
		m_sessionToServiceMap.erase(sessionID);
	}

	OT_LOG_D("Session was closed (ID = \"" + sessionID + "\")");

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string GlobalSessionService::handleForceHealthcheck(ot::JsonDocument& _doc) {
	if (m_healthCheckRunning) {
		m_forceHealthCheck = true;
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else return OT_ACTION_RETURN_VALUE_FAILED;
}

std::string GlobalSessionService::handleNewGlobalDirectoryService(ot::JsonDocument& _doc) {
	// Check document
	if (!_doc.HasMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL)) {
		OT_LOG_E("Missing \"" OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL "\" member");
		return OT_ACTION_RETURN_INDICATOR_Error "Missing \"" OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL "\" member";
	}
	if (!_doc[OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL].IsString()) {
		OT_LOG_E("Member \"" OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL "\" is not a string");
		return OT_ACTION_RETURN_INDICATOR_Error "Member \"" OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL "\" is not a string";
	}

	// Set new GDS url
	if (!m_globalDirectoryUrl.empty()) {
		OT_LOG_D("Removing old Global Directory Service (url = " + m_globalDirectoryUrl + ")");
	}
	m_globalDirectoryUrl = _doc[OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL].GetString();

	OT_LOG_D("Set new Global Directory Service (url = " + m_globalDirectoryUrl + ")");

	// Create notification document for the local session services
	ot::JsonDocument lssDoc;
	lssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, lssDoc.GetAllocator()), lssDoc.GetAllocator());
	lssDoc.AddMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL, ot::JsonString(m_globalDirectoryUrl, lssDoc.GetAllocator()), lssDoc.GetAllocator());

	// Notify connected Local Session Services
	std::lock_guard<std::mutex> lock(m_mapMutex);

	std::string response;
	for (auto ss : m_sessionServiceIdMap) {
		if (!ot::msg::send("", ss.second->url(), ot::EXECUTE, lssDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			OT_LOG_EAS("Failed to send message to Local Session Service (url = " + ss.second->url() + ")");
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to send message to Local Session Service (url = " + ss.second->url() + ")";
		}
		if (response != OT_ACTION_RETURN_VALUE_OK) {
			OT_LOG_EAS("Invalid response from LSS (url = " + ss.second->url() + "): " + response);
		}
	}
	
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string GlobalSessionService::handleSetGlobalLogFlags(ot::JsonDocument& _doc) {
	ot::ConstJsonArray flags = ot::json::getArray(_doc, OT_ACTION_PARAM_Flags);
	m_logModeManager.setGlobalLogFlags(ot::logFlagsFromJsonArray(flags));

	ot::LogDispatcher::instance().setLogFlags(m_logModeManager.getGlobalLogFlags());

	// Update existing session services
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_SetGlobalLogFlags, doc.GetAllocator());
	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(m_logModeManager.getGlobalLogFlags(), flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Flags, flagsArr, doc.GetAllocator());
	
	std::string json = doc.toJson();
	for (const auto& it : m_sessionServiceIdMap) {
		std::string response;
		if (!ot::msg::send("", it.second->url(), ot::EXECUTE, json, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			OT_LOG_EAS("Failed to send message to LSS at \"" + it.second->url() + "\"");
		}
	}

	std::string tmp;
	if (!ot::msg::send("", m_globalDirectoryUrl, ot::EXECUTE, json, tmp, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_EAS("Failed to send message to GDS at \"" + m_globalDirectoryUrl + "\"");
	}

	if (!ot::msg::send("", m_authorizationUrl, ot::EXECUTE, json, tmp, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_EAS("Failed to send message to Authorization Service at \"" + m_authorizationUrl + "\"");
	}

	if (!ot::ServiceLogNotifier::instance().loggingServiceURL().empty()) {
		if (!ot::msg::send("", ot::ServiceLogNotifier::instance().loggingServiceURL(), ot::EXECUTE, json, tmp, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			OT_LOG_EAS("Failed to send message to Logging Service at \"" + ot::ServiceLogNotifier::instance().loggingServiceURL() + "\"");
		}
	}
	
	return OT_ACTION_RETURN_VALUE_OK;
}

// ###################################################################################################

// Private functions

void GlobalSessionService::healthCheck() {
	const static int limit{ 60 }; // 1 min
	int counter{ 0 };

	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string pingMessage = pingDoc.toJson();

	while (m_healthCheckRunning) {
		counter = 0;
		while (counter++ < limit && m_healthCheckRunning && !m_forceHealthCheck)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
		m_forceHealthCheck = false;
		if (m_healthCheckRunning) {
			std::lock_guard<std::mutex> lock(m_mapMutex);

			bool running{ true };
			// Use running in case of a disconnected service
			while (running) {
				running = false;
				for (auto it : m_sessionServiceIdMap) {
					
					OT_LOG_D("Performing health check for LSS (url = " + it.second->url() + ")");

					try {
						std::string response;
						// Try to ping
						if (!ot::msg::send(m_url, it.second->url(), ot::EXECUTE, pingMessage, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
							OT_LOG_W("Ping could not be delivered to LSS (url = " + it.second->url() + ")");
							removeSessionService(it.second);
							running = true;
							break;
						}
						// Check ping response
						if (response != OT_ACTION_CMD_Ping) {
							OT_LOG_W("Invalid ping response from LSS (url = " + it.second->url() + ")");
							removeSessionService(it.second);
							running = true;
							break;
						}
					}
					catch (const std::exception& e) {
						OT_LOG_E("Health check for LSS (url = " + it.second->url() + ") failed: " + e.what());
						removeSessionService(it.second);
						running = true;
						break;
					}
					catch (...) {
						OT_LOG_E("[FATAL] Health check for LSS (url = " + it.second->url() + ") failed: Unknown error");
						removeSessionService(it.second);
						running = true;
						break;
					}
				}
			}
		}
	}
}

void GlobalSessionService::removeSessionService(LocalSessionService * _service) {
	if (_service) {
		// Service UID map
		m_sessionServiceIdMap.erase(_service->id());

		// Session to Service map
		bool erased{ true };
		while (erased) {
			erased = false;
			for (auto it : m_sessionToServiceMap) {
				if (it.second == _service) {
					erased = true;
					m_sessionToServiceMap.erase(it.first);
					break;
				}
			}
		}
		delete _service;
	}
}

LocalSessionService * GlobalSessionService::leastLoadedSessionService(void) {
	LocalSessionService * leastLoadedSessionService{ nullptr };
	size_t minLoad{ UINT64_MAX };
	for (auto it : m_sessionServiceIdMap) {
		if (it.second->sessionCount() < minLoad) {
			leastLoadedSessionService = it.second;
			minLoad = leastLoadedSessionService->sessionCount();
		}
	}
	return leastLoadedSessionService;
}

void GlobalSessionService::getCustomProjectTemplates(ot::JsonDocument& _resultArray, const std::string& _user, const std::string& _password) {
	try {
		DataStorageAPI::ConnectionAPI::establishConnection(m_databaseUrl, "1", _user, _password);

		// Now we run a command on the server and check whether its is really responding to us (the following command throws an exception if not)
		if (!DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists("Projects", "Catalog")) {
			OT_LOG_E("Projects/Catalog collection does not exist");
			return;
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		return;
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error");
		return;
	}

	mongocxx::database db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("ProjectTemplates");

	std::vector<std::string> names = db.list_collection_names();

	for (const std::string& name : names) {
		try {
			std::string projectType = OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT;
			std::string description;
			std::string briefDescription;

			if (!DataStorageAPI::ConnectionAPI::getInstance().checkCollectionExists("ProjectTemplates", name)) {
				OT_LOG_EAS("Template collection \"" + name + "\" can not be accessed");
				continue;
			}

			// Get template information
			DataStorageAPI::DocumentAccess docManager("ProjectTemplates", name);

			auto queryDoc = bsoncxx::builder::basic::document{};
			queryDoc.append(bsoncxx::builder::basic::kvp("Information", ""));

			auto filterDoc = bsoncxx::builder::basic::document{};

			auto result = docManager.GetDocument(std::move(queryDoc.extract()), std::move(filterDoc.extract()));

			if (!result.getSuccess()) {
				OT_LOG_EAS("Grabbing template collection \"" + name + "\" failed");
				continue;
			}

			bsoncxx::builder::basic::document doc;
			doc.append(bsoncxx::builder::basic::kvp("Found", result.getBsonResult().value()));

			auto doc_view = doc.view()["Found"].get_document().view();

			try {
				projectType = doc_view["ProjectType"].get_utf8().value.data();
			}
			catch (...) {}
			try {
				description = doc_view["Description"].get_utf8().value.data();
			}
			catch (...) {}
			try {
				briefDescription = doc_view["BriefDescription"].get_utf8().value.data();
			}
			catch (...) {}

			// Create and serialize new information
			ot::ProjectTemplateInformation newInfo;
			newInfo.setName(name);
			newInfo.setIsDefault(false);
			newInfo.setProjectType(projectType);
			newInfo.setBriefDescription(briefDescription);

			ot::StyledTextBuilder descriptionBuilder;
			bool descriptionOk = false;
			if (description.find('{') == 0) {
				ot::JsonDocument descriptionDoc;
				if (descriptionDoc.fromJson(description)) {
					if (descriptionDoc.IsObject()) {
						descriptionBuilder.setFromJsonObject(descriptionDoc.GetConstObject());
						descriptionOk = true;
					}
				}
			}
			if (!descriptionOk) {
				descriptionBuilder << description;
			}
			newInfo.setDescription(descriptionBuilder);

			ot::JsonObject obj;
			newInfo.addToJsonObject(obj, _resultArray.GetAllocator());
			_resultArray.PushBack(obj, _resultArray.GetAllocator());
		}
		catch (...) {
			OT_LOG_E("Any request for template collection \"" + name + "\" failed");
		}
	}
}

GlobalSessionService::GlobalSessionService()
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalSessionService, OT_INFO_SERVICE_TYPE_GlobalSessionService),
	m_healthCheckRunning(false), m_forceHealthCheck(false)
{
	m_SystemLoadInformation.initialize();
}

GlobalSessionService::~GlobalSessionService() {

}