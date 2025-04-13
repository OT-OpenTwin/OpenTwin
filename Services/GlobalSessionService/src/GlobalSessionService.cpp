//! @file GlobalSessionService.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

#define NOMINMAX //Supress windows macros min max
#define WIN32_LEAN_AND_MEAN

// GSS header
#include "Session.h"
#include "LocalSessionService.h"
#include "GlobalSessionService.h"

// OpenTwin header
#include "OTSystem/SystemInformation.h"
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/ContainerHelper.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ServiceLogNotifier.h"
#include "DataBase.h"

#include "DocumentAPI.h"
#include "Connection\ConnectionAPI.h"
#include "Document\DocumentAccess.h"

// Third party header
#include "zlib.h"
#include "base64.h"

// std header
#include <thread>
#include <chrono>
#include <limits>
#include <fstream>
#include <iostream>
#include <filesystem>

GlobalSessionService& GlobalSessionService::instance(void) {
	static GlobalSessionService g_instance;
	return g_instance;
}

// ###################################################################################################

// Service handling

bool GlobalSessionService::addSessionService(LocalSessionService&& _service) {
	// Check URL duplicate
	for (auto& it : m_lssMap) {
		if (it.second.getUrl() == _service.getUrl()) {
			OT_LOG_WAS("LSS with given url already registered. Url: \"" + _service.getUrl() + "\"");
			return true;
		}
	}

	// Create copy and assign uid
	ot::serviceID_t newID = m_lssIdManager.grabNextID();
	_service.setId(newID);

	OT_LOG_D("New LSS registered. { \"ID\": " + std::to_string(_service.getId()) + ", \"URL\": \"" + _service.getUrl() + "\" }");

	// Register already opened sessions
	for (const std::string& session : _service.getSessionIds()) {
		m_sessionMap.insert_or_assign(session, _service.getId());
		OT_LOG_D("Adding already running session: \"" + session + "\"");
	}
	m_lssMap.insert_or_assign(newID, std::move(_service));

	// Check if the health check is aready running, otherwise start it
	if (!m_workerRunning) {
		m_workerRunning = true;

		std::thread healthWorker(&GlobalSessionService::workerHealthCheck, this);
		healthWorker.detach();

		std::thread iniWorker(&GlobalSessionService::workerSessionIni, this);
		iniWorker.detach();
	}

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void GlobalSessionService::setDatabaseUrl(const std::string& _url) {
	 m_databaseUrl = _url;
	 if (m_databaseUrl.find("tls@") != std::string::npos)
	 {
		 m_databaseUrl = m_databaseUrl.substr(4);
	 }
}

// ###################################################################################################

// Action handler

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

	std::lock_guard<std::mutex> lock(m_mutex);

	auto it = m_sessionMap.find(newSession.getId());
	if (it != m_sessionMap.end()) {
		// Session already open
		const LocalSessionService& lss = this->getLssFromSessionId(newSession.getId());

		if (newSession.getUserName() == lss.getSessionUser(newSession.getId())) {
			// Session open by same user in a different instance
			OT_LOG_WAS("Session already opened by same user. Session: \"" + newSession.getId() + "\"");
			ot::ReturnMessage response(ot::ReturnMessage::Failed, "Session already open in another instance");
			return response.toJson();
		}
		else {
			// Session open by different user
			OT_LOG_WAS("Session already opened by other user. Session: \"" + newSession.getId() + "\"");
			ot::ReturnMessage response(ot::ReturnMessage::Failed, "Session open by other user");
			return response.toJson();
		}
	}
	else {
		// New session

		// Determine LSS
		LocalSessionService* lss = determineLeastLoadedLSS();
		if (!lss) {
			ot::ReturnMessage response(ot::ReturnMessage::Failed, "Failed to determine least loaded LSS");
			OT_LOG_EAS(response.getWhat());
			return response.toJson();
		}

		// Store session for the given LSS
		OT_LOG_D("LSS determined { \"URL\": \"" + lss->getUrl() + "\", \"ID\": " + std::to_string(lss->getId()) + ", \"Session\": \"" + newSession.getId() + "\" }");
		m_sessionMap.insert_or_assign(newSession.getId(), lss->getId());

		// Add session to the LSS ini list
		lss->addIniSession(std::move(newSession));

		ot::ReturnMessage response(ot::ReturnMessage::Ok, lss->getUrl());
		return response.toJson();
	}
}

std::string GlobalSessionService::handleConfirmSession(ot::JsonDocument& _doc) {
	std::string sessionID = ot::json::getString(_doc, OT_ACTION_PARAM_SESSION_ID);

	std::lock_guard<std::mutex> lock(m_mutex);


	ot::ReturnMessage response(ot::ReturnMessage::Ok);
	return response.toJson();
}

std::string GlobalSessionService::handleCheckProjectOpen(ot::JsonDocument& _doc) {
	std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);

	std::lock_guard<std::mutex> lock(m_mutex);

	auto it = m_sessionMap.find(projectName);
	if (it == m_sessionMap.end()) {
		// Session does not exist
		return std::string();
	}
	else {
		try {
			// Get user (may throw if not found...)
			return this->getLssFromSessionId(projectName).getSessionUser(projectName);
		}
		catch (const std::exception& _e) {
			OT_LOG_E(_e.what());
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

std::string GlobalSessionService::handleGetBuildInformation(ot::JsonDocument& _doc) {
	ot::SystemInformation info;
	std::string buildInfo = info.getBuildInformation();

	return buildInfo;
}

std::string GlobalSessionService::handlePrepareFrontendInstaller(ot::JsonDocument& _doc) {
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

std::string GlobalSessionService::handleGetSystemInformation(ot::JsonDocument& _doc) {

	double globalCpuLoad = 0, globalMemoryLoad = 0;
	m_systemLoadInformation.getGlobalCPUAndMemoryLoad(globalCpuLoad, globalMemoryLoad);

	double processCpuLoad = 0, processMemoryLoad = 0;
	m_systemLoadInformation.getCurrentProcessCPUAndMemoryLoad(processCpuLoad, processMemoryLoad);

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_CPU_LOAD, globalCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_GLOBAL_MEMORY_LOAD, globalMemoryLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_CPU_LOAD, processCpuLoad, reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_PROCESS_MEMORY_LOAD, processMemoryLoad, reply.GetAllocator());

	// Now we add information about the session services
	ot::JsonArray servicesInfo;

	for (auto& it : m_lssMap) {
		ot::JsonValue info;
		info.SetObject();

		info.AddMember(OT_ACTION_PARAM_LSS_URL, ot::JsonString(it.second.getUrl(), reply.GetAllocator()), reply.GetAllocator());

		servicesInfo.PushBack(info, reply.GetAllocator());
	}

	reply.AddMember(OT_ACTION_PARAM_LSS, servicesInfo, reply.GetAllocator());

	return reply.toJson();
}

std::string GlobalSessionService::handleRegisterSessionService(ot::JsonDocument& _doc) {
	LocalSessionService nService;
	// Gather information from document
	nService.setFromJsonObject(_doc.GetConstObject());
	
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_SERVICE_ID, nService.getId(), reply.GetAllocator());

	// Add to GSS
	if (!this->addSessionService(std::move(nService))) { 
		return OT_ACTION_RETURN_INDICATOR_Error "Failed to attach service information";
	}

	// Finish creating reply message
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

	std::lock_guard<std::mutex> lock(m_mutex);

	auto id = m_sessionMap.find(sessionID);
	if (id != m_sessionMap.end()) {
		auto lss = m_lssMap.find(id->second);
		if (lss == m_lssMap.end()) {
			OT_LOG_EA("LSS data mismatch");
			throw std::out_of_range("LSS entry not found for existing session");
		}

		lss->second.closeSession(sessionID);

		m_sessionMap.erase(sessionID);
	}

	OT_LOG_D("Session was closed (ID = \"" + sessionID + "\")");

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string GlobalSessionService::handleForceHealthcheck(ot::JsonDocument& _doc) {
	if (m_workerRunning) {
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
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string response;
	for (auto& lss : m_lssMap) {
		if (!ot::msg::send("", lss.second.getUrl(), ot::EXECUTE, lssDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			OT_LOG_EAS("Failed to send message to Local Session Service (url = " + lss.second.getUrl() + ")");
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to send message to Local Session Service (url = " + lss.second.getUrl() + ")";
		}
		if (response != OT_ACTION_RETURN_VALUE_OK) {
			OT_LOG_EAS("Invalid response from LSS (url = " + lss.second.getUrl() + "): " + response);
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
	for (const auto& it : m_lssMap) {
		std::string response;
		if (!ot::msg::send("", it.second.getUrl(), ot::EXECUTE, json, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			OT_LOG_EAS("Failed to send message to LSS at \"" + it.second.getUrl() + "\"");
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

// Private

GlobalSessionService::GlobalSessionService() :
	ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalSessionService, OT_INFO_SERVICE_TYPE_GlobalSessionService),
	m_workerRunning(false), m_forceHealthCheck(false)
{
	m_systemLoadInformation.initialize();
}

GlobalSessionService::~GlobalSessionService() {

}

void GlobalSessionService::removeSessionService(const LocalSessionService& _service) {
	for (const std::string& session : _service.getSessionIds()) {
		m_sessionMap.erase(session);
	}

	m_lssMap.erase(_service.getId());
}

LocalSessionService& GlobalSessionService::getLssFromSessionId(const std::string& _sessionId) {
	auto id = m_sessionMap.find(_sessionId);
	if (id == m_sessionMap.end()) {
		OT_LOG_E("Session was not registered. Session: \"" + _sessionId + "\"");
		throw std::out_of_range("Session was not registered.  Session: \"" + _sessionId + "\"");
	}

	auto lss = m_lssMap.find(id->second);
	if (lss == m_lssMap.end()) {
		OT_LOG_E("Session service not found for registered session. LSS.ID: " + std::to_string(id->second) + ". Session: \"" + _sessionId + "\".");
		throw std::out_of_range("Session service not found for registered session. LSS.ID: " + std::to_string(id->second) + ". Session: \"" + _sessionId + "\".");
	}

	return lss->second;
}

const LocalSessionService& GlobalSessionService::getLssFromSessionId(const std::string& _sessionId) const {
	const auto id = m_sessionMap.find(_sessionId);
	if (id == m_sessionMap.end()) {
		OT_LOG_E("Session was not registered. Session: \"" + _sessionId + "\"");
		throw std::out_of_range("Session was not registered.  Session: \"" + _sessionId + "\"");
	}

	const auto lss = m_lssMap.find(id->second);
	if (lss == m_lssMap.end()) {
		OT_LOG_E("Session service not found for registered session. LSS.ID: " + std::to_string(id->second) + ". Session: \"" + _sessionId + "\".");
		throw std::out_of_range("Session service not found for registered session. LSS.ID: " + std::to_string(id->second) + ". Session: \"" + _sessionId + "\".");
	}

	return lss->second;
}

LocalSessionService* GlobalSessionService::determineLeastLoadedLSS(void) {
	LocalSessionService* leastLoadedSessionService{ nullptr };
	size_t minLoad{ UINT64_MAX };
	for (auto& it : m_lssMap) {
		if (it.second.getSessionCount() < minLoad) {
			leastLoadedSessionService = &it.second;
			minLoad = leastLoadedSessionService->getSessionCount();
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

// ###########################################################################################################################################################################################################################################################################################################################

// Installer

void GlobalSessionService::readFileContent(const std::string& fileName, std::string& fileContent) {
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

void GlobalSessionService::loadFrontendInstallerFile() {
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

// ###########################################################################################################################################################################################################################################################################################################################

// Worker

void GlobalSessionService::workerHealthCheck(void) {
	const static int limit{ 60 }; // 1 min
	int counter{ 0 };

	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string pingMessage = pingDoc.toJson();

	while (m_workerRunning) {
		counter = 0;
		while (counter++ < limit && m_workerRunning && !m_forceHealthCheck) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
		m_forceHealthCheck = false;
		if (m_workerRunning) {
			std::lock_guard<std::mutex> lock(m_mutex);

			bool running{ true };
			// Use running in case of a disconnected service
			while (running) {
				running = false;
				for (auto& it : m_lssMap) {

					OT_LOG_D("Performing health check for LSS (url = " + it.second.getUrl() + ")");

					try {
						std::string response;
						// Try to ping
						if (!ot::msg::send(m_url, it.second.getUrl(), ot::EXECUTE, pingMessage, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
							OT_LOG_W("Ping could not be delivered to LSS (url = " + it.second.getUrl() + ")");
							removeSessionService(it.second);
							running = true;
							break;
						}
						// Check ping response
						if (response != OT_ACTION_CMD_Ping) {
							OT_LOG_W("Invalid ping response from LSS (url = " + it.second.getUrl() + ")");
							removeSessionService(it.second);
							running = true;
							break;
						}
					}
					catch (const std::exception& e) {
						OT_LOG_E("Health check for LSS (url = " + it.second.getUrl() + ") failed: " + e.what());
						removeSessionService(it.second);
						running = true;
						break;
					}
					catch (...) {
						OT_LOG_E("[FATAL] Health check for LSS (url = " + it.second.getUrl() + ") failed: Unknown error");
						removeSessionService(it.second);
						running = true;
						break;
					}
				}
			}
		}
	}
}

void GlobalSessionService::workerSessionIni(void) {
	while (m_workerRunning) {
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			
			std::list<Session> timedOut;

			auto lssIdList = ot::ContainerHelper::listFromMapValues(m_sessionMap);
			lssIdList.unique();

			std::list<LocalSessionService*> lssList;

			for (auto id : lssIdList) {
				auto it = m_lssMap.find(id);
				if (it == m_lssMap.end()) {
					OT_LOG_E("LSS data mismatch");
					continue;
				}
				lssList.push_back(&it->second);
			}

			// Check all sessions
			for (LocalSessionService* lss : lssList) {
				timedOut.splice(timedOut.end(), lss->checkTimedOutIniSessions(1 * ot::msg::defaultTimeout));
			}

			// Remove all sessions that timed out
			for (Session& session : timedOut) {
				OT_LOG_E("Session timed out while wating for LSS confirmation. Unlocking session. Session: \"" + session.getId() + "\"");

				// Remove from map, LSS already removed session from its list
				m_sessionMap.erase(session.getId());
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

