//! @file SessionService.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LSS header
#include "Session.h"
#include "GlobalSessionService.h"
#include "GlobalDirectoryService.h"

// OpenTwin header
#include "OTSystem/PortManager.h"
#include "OTSystem/SystemInformation.h"
#include "OTCore/LogTypes.h"
#include "OTCore/ServiceBase.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"

// std header
#include <map>
#include <list>
#include <string>
#include <unordered_set>

class SessionService {
	OT_DECL_NOCOPY(SessionService)
	OT_DECL_NOMOVE(SessionService)
	OT_DECL_ACTION_HANDLER(SessionService)
private:
	SessionService();
	~SessionService() {};
public:
	static SessionService& instance();
	static int initialize(const std::string& _ownUrl, const std::string& _gssUrl);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	const std::string& getUrl() const { return m_url; };

	void setSiteID(std::string _id) { m_siteID = _id; };
	const std::string& getSiteID() const { return m_siteID; };

	std::list<std::string> getSessionIDs();

	void serviceFailure(const std::string& _sessionID, ot::serviceID_t _serviceID);

private:

	bool getIsServiceInDebugMode(const std::string& _serviceName);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Management

	Session createSession(const std::string& _sessionID, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _sessionType);

	Session& getSession(const std::string& _sessionID);

	bool runMandatoryServices(Session& _session);

	void updateLogMode(const ot::LogFlags& _newData);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	Service& runServiceInDebug(const ot::ServiceBase& _serviceInfo, Session& _session);

	Service& runRelayService(Session& _session, const std::string& _serviceName, const std::string& _serviceType);

	bool hasMandatoryService(const std::string& _serviceName) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	void workerShutdownSession();
	void workerRunServices();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	OT_HANDLER(handleGetDBURL, SessionService, OT_ACTION_CMD_GetDatabaseUrl, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleGetAuthURL, SessionService, OT_ACTION_CMD_GetAuthorisationServerUrl, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleGetGlobalServicesURL, SessionService, OT_ACTION_CMD_GetGlobalServicesUrl, ot::ALL_MESSAGE_TYPES)

	OT_HANDLER(handleGetSystemInformation, SessionService, OT_ACTION_CMD_GetSystemInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetMandatoryServices, SessionService, OT_ACTION_CMD_GetMandatoryServices, ot::SECURE_MESSAGE_TYPES)

	OT_HANDLER(handleCreateNewSession, SessionService, OT_ACTION_CMD_CreateNewSession, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCheckProjectOpen, SessionService, OT_ACTION_CMD_IsProjectOpen, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleConfirmService, SessionService, OT_ACTION_CMD_ConfirmService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleShowService, SessionService, OT_ACTION_CMD_ShowService, ot::SECURE_MESSAGE_TYPES)

	OT_HANDLER(handleGetSessionExists, SessionService, OT_ACTION_CMD_GetSessionExists, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceClosing, SessionService, OT_ACTION_CMD_ServiceClosing, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleShutdownSession, SessionService, OT_ACTION_CMD_ShutdownSession, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceFailure, SessionService, OT_ACTION_CMD_ServiceFailure, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceShutdownCompleted, SessionService, OT_ACTION_CMD_ServiceShutdownCompleted, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleEnableServiceDebug, SessionService, OT_ACTION_CMD_ServiceEnableDebug, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleDisableServiceDebug, SessionService, OT_ACTION_CMD_ServiceDisableDebug, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetDebugInformation, SessionService, OT_ACTION_CMD_GetDebugInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleCheckStartupCompleted, SessionService, OT_ACTION_CMD_CheckStartupCompleted, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleAddMandatoryService, SessionService, OT_ACTION_CMD_AddMandatoryService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleRegisterNewGlobalDirectoryService, SessionService, OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceStartupFailed, SessionService, OT_ACTION_CMD_ServiceStartupFailed, ot::SECURE_MESSAGE_TYPES)

	OT_HANDLER(handleSetGlobalLogFlags, SessionService, OT_ACTION_CMD_SetGlobalLogFlags, ot::SECURE_MESSAGE_TYPES)

	std::mutex                                        m_mutex;
	std::atomic_bool                                  m_workerRunning;

	GlobalSessionService                              m_gss;
	GlobalDirectoryService                            m_gds;

	std::string                                       m_siteID; //! @brief Site ID

	std::string                                       m_url;
	ot::serviceID_t                                   m_id;
	ot::SystemInformation                             m_systemLoadInformation;

	std::unordered_set<std::string>                   m_debugServices;

	std::map<std::string, Session>                    m_sessions;
	std::map<std::string, std::list<ot::ServiceBase>> m_mandatoryServicesMap; //! @brief Map containing all names of mandatory services for each session type

	ot::PortManager                                   m_debugPortManager;
};
