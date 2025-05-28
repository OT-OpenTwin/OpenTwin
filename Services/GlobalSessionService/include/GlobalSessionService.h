//! @file GlobalSessionService.h
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/LogModeManager.h"
#include "OTGui/ProjectTemplateInformation.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"

#include "OTServiceFoundation/IDManager.h"
#include "OTSystem/SystemInformation.h"

// C++ header
#include <string>
#include <list>
#include <map>
#include <mutex>

class LocalSessionService;

//! @class GlobalSessionService
//! @brief The GlobalSessionService is the central class of this service.
//! It is responsible for managing all LocalSessionServices and the sessions in them.
class GlobalSessionService : public ot::ServiceBase, public ot::Serializable {
	OT_DECL_ACTION_HANDLER(GlobalSessionService)
	OT_DECL_NOCOPY(GlobalSessionService)
	OT_DECL_NOMOVE(GlobalSessionService)
public:
	//! @brief Returns the global instance of the GlobalSessionService.
	static GlobalSessionService& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service handling

	//! @brief Add the session service to the list of session services.
	//! @param _service The session service to add.
	//! @param _newId The new id assigned to the session service.
	//! @return True if the service was added successfully, false otherwise.
	bool addSessionService(LocalSessionService&& _service, ot::serviceID_t& _newId);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setDatabaseUrl(const std::string& _url);
	const std::string getDatabaseUrl(void) const { return m_databaseUrl; };

	void setAuthorizationUrl(const std::string& _url) { m_authorizationUrl = _url; };
	const std::string& getAuthorizationUrl(void) const { return m_authorizationUrl; };

	void setGlobalDirectoryUrl(const std::string& _url) { m_globalDirectoryUrl = _url; };
	const std::string& getGlobalDirectoryUrl(void) const { return m_globalDirectoryUrl; };

private:

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	OT_HANDLER(handleGetDBUrl, GlobalSessionService, OT_ACTION_CMD_GetDatabaseUrl, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleGetGlobalServicesURL, GlobalSessionService, OT_ACTION_CMD_GetGlobalServicesUrl, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCreateSession, GlobalSessionService, OT_ACTION_CMD_CreateNewSession, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleConfirmSession, GlobalSessionService, OT_ACTION_CMD_ConfirmSession, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCheckProjectOpen, GlobalSessionService, OT_ACTION_CMD_IsProjectOpen, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleGetProjectTemplatesList, GlobalSessionService, OT_ACTION_CMD_GetListOfProjectTemplates, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleGetBuildInformation, GlobalSessionService, OT_ACTION_CMD_GetBuildInformation, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleGetFrontendInstaller, GlobalSessionService, OT_ACTION_CMD_GetFrontendInstaller, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handlePrepareFrontendInstaller, GlobalSessionService, OT_ACTION_CMD_PrepareFrontendInstaller, ot::ALL_MESSAGE_TYPES)

	OT_HANDLER(handleGetDebugInformation, GlobalSessionService, OT_ACTION_CMD_GetDebugInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetSystemInformation, GlobalSessionService, OT_ACTION_CMD_GetSystemInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleRegisterSessionService, GlobalSessionService, OT_ACTION_CMD_RegisterNewSessionService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleRegisterLibraryManagementService, GlobalSessionService, OT_ACTION_CMD_RegisterNewLibraryManagementService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleShutdownSession, GlobalSessionService, OT_ACTION_CMD_ShutdownSessionCompleted, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleForceHealthcheck, GlobalSessionService, OT_ACTION_CMD_ForceHealthCheck, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleNewGlobalDirectoryService, GlobalSessionService, OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSetGlobalLogFlags, GlobalSessionService, OT_ACTION_CMD_SetGlobalLogFlags, ot::SECURE_MESSAGE_TYPES)

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private
	
	GlobalSessionService();
	~GlobalSessionService();

	//! @brief Will remove the service from the service map aswell as all sessions in this service
	void removeSessionService(const LocalSessionService& _service);

	//! @brief Returns the LSS associated with the session id.
	//! Will throw an exception if the LSS was not found.
	LocalSessionService& getLssFromSessionId(const std::string& _sessionId);

	//! @brief Returns the LSS associated with the session id.
	//! Will throw an exception if the LSS was not found.
	const LocalSessionService& getLssFromSessionId(const std::string& _sessionId) const;

	//! @brief Determins the least loaded LSS.
	//! The load is determined by the number of sessions in the LSS.
	LocalSessionService* determineLeastLoadedLSS(void);

	//! @brief Returns the custom project templates accessible by the user.
	//! @param _resultArray Results array to fill with the project templates.
	//! @param _user User name to use for authentication.
	//! @param _password Password to use for authentication.
	void getCustomProjectTemplates(ot::JsonDocument& _resultArray, const std::string& _user, const std::string& _password);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Installer

	//! @brief Read contents of the compressed file into the provided string.
	//! @param fileName Name of the file containing the compressed installer.
	//! @param fileContent Resulting string containing the decompressed file contents.
	void readFileContent(const std::string& fileName, std::string& fileContent);

	//! @brief Search for the installer and read the file.
	//! First the installer will be searched at "<Current Path>/FrontendInstaller".
	//! If not found, the installer will be searched at "%OPENTWIN_DEV_ROOT%/Deployment/FrontendInstaller".
	void loadFrontendInstallerFile(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Worker

	//! @brief Worker that ensures all the registered LSS are alive.
	void workerHealthCheck(void);

	//! @brief Worker ensuring all sessions are confirmed in time by the corresponding LSS.
	void workerSessionIni(void);

	std::string m_databaseUrl;                //! @brief Database url.
	std::string m_authorizationUrl;           //! @brief Authorization Service url.
	std::string m_globalDirectoryUrl;         //! @brief Global Directory Service url.
	
	std::atomic_bool m_workerRunning;         //! @brief If true the workers are running, otherwise they still might be shutting down but will terminate if not set back to true in time.
	std::atomic_bool m_forceHealthCheck;      //! @brief If true the next health check cycle will perform a health check even if the timeout of the next health check is not reached yet.

	std::mutex									m_mutex;                 //! @brief Central mutex protecting all shared data.
	std::map<std::string, ot::serviceID_t>		m_sessionMap;            //! @brief Session ID to LSS ID map.
	std::map<ot::serviceID_t, LocalSessionService>	m_lssMap;            //! @brief LSS ID to LSS map.
	ot::IDManager<ot::serviceID_t>				m_lssIdManager;          //! @brief ID generator used to assign IDs to the LSS.
	ot::SystemInformation						m_systemLoadInformation; //! @brief Current system load information.

	ot::LogModeManager m_logModeManager;                                 //! @brief Log mode manager.
	std::string m_frontendInstallerFileContent;                          //! @brief Contents of the installer that will be provided to the frontend.
};