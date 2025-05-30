//! @file SessionService.h
//! @author Alexander Kuester (alexk95)
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LSS header
#include "Service.h"
#include "GlobalSessionService.h"
#include "GlobalDirectoryService.h"

// OpenTwin header
#include "OTSystem/SystemInformation.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/LogModeManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"

// std header
#include <map>
#include <vector>
#include <string>

class SessionService {
	OT_DECL_ACTION_HANDLER(SessionService)
private:
	SessionService();
	~SessionService() {};
public:
	static SessionService& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Management

	void connectToGDS(const std::string& _url);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Session Information

	std::list<std::string> getSessionIds(void);

private:
	GlobalSessionService   m_gss;
	GlobalDirectoryService m_gds;




public:



















	// ######################################################################################

	// Setter / Getter

	void setIp(const std::string& _ip) { m_ip = _ip; };
	const std::string& getIp(void) const { return m_ip; };

	void setPort(const std::string& _port);
	const std::string& getPort(void) const { return m_port; };

	void setId(ot::serviceID_t _id) { m_id = _id; };
	ot::serviceID_t getId(void) const { return m_id; };

	std::string getUrl(void) const { return m_ip + ":" + m_port; };

	void setDataBaseURL(const std::string& _url) { m_dataBaseURL = _url; }
	const std::string& getDataBaseURL(void) const { return m_dataBaseURL; }

	void setSiteID(std::string _ID) { m_siteID = _ID; }
	const std::string& getSiteID(void) const { return m_siteID; }

	void setAuthorisationServiceURL(const std::string& _url) { m_serviceAuthorisationURL = _url; }
	const std::string& getServiceAuthorisationURL(void) const { return m_serviceAuthorisationURL; }

	bool getIsServiceInDebugMode(const std::string& _serviceName);

	// ######################################################################################

	// Service management

	//! @brief Will create a new session with the provided session information
	//! Will return a pointer to the new session, the session is stored in the sessionService
	//! @param _sessionID The ID of the new session (must be unique)
	//! @param _userName The name of the user that started this session
	//! @param _projectName The namne of the project used in this session
	//! @param _collectionName The name of the collection used in this session
	//! @param _sessionType The type of the session
	//! @param _runMandatoryServices If true, all mandatory services for the specified session type will be started
	Session* createSession(const std::string& _sessionID, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _sessionType);

	//! @brief Returns the session with the specified id
	//! @param _sessionID The ID of the session to get
	//! @param _throwException If true, exceptions will be thrown, otherwise a nullptr will be returned
	//! @throw std::exception if the specified session ID is invalid
	Session * getSession(const std::string& _sessionID);

	//! @brief Will ensure that all mandatory services for the specified session are running
	//! @param _sessionID The ID of the session
	bool runMandatoryServices(const std::string& _sessionID);

	//! @brief Will ensure that all mandatory services for the specified session are running
	//! @return false if a debug or release service could not be handled
	bool runMandatoryServices(Session* _session);

	//! @brief Will close the service and deregister it from its session
	void serviceClosing(Service* _service, bool _notifyOthers, bool _autoCloseSessionIfMandatory = true);

	//! @brief Will remove the session and close its logger.
	//! Will NOT perform the session shutdown().
	//! Will clear the memory of the provided pointer.
	//! @param _session The session to remove
	void removeSession(Session* _session);

	//! \brief Removes the session from the service run starter and all maps.
	void forgetSession(Session* _session);

	std::list<const Session *> sessions(void);
	
	//! @brief Create a config for the service that shall be started in Debug mode
	//! @return false if the OPENTWIN_DEV_ROOT is not set since it is needed for the config path, otherwise true
	bool runServiceInDebug(const std::string& _serviceName, const std::string& _serviceType, Session* _session, std::string& _serviceURL);
	
	bool runRelayService(Session * _session, std::string& _websocketURL, std::string& _serviceURL);

	void setGlobalSessionService(GlobalSessionService * _gss) { m_gss = _gss; }

	Service * getServiceFromURL(const std::string& _url);

	//! @brief Initialize the collection of system information (e.g. CPU time and memory).
	void initializeSystemInformation();

	void updateLogMode(const ot::LogModeManager& _newData);

private:
	// Message handling

	OT_HANDLER(handleGetDBURL, SessionService, OT_ACTION_CMD_GetDatabaseUrl, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleGetAuthURL, SessionService, OT_ACTION_CMD_GetAuthorisationServerUrl, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleGetGlobalServicesURL, SessionService, OT_ACTION_CMD_GetGlobalServicesUrl, ot::ALL_MESSAGE_TYPES)

	OT_HANDLER(handleGetSystemInformation, SessionService, OT_ACTION_CMD_GetSystemInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetMandatoryServices, SessionService, OT_ACTION_CMD_GetMandatoryServices, ot::SECURE_MESSAGE_TYPES)

	OT_HANDLER(handleRegisterNewService, SessionService, OT_ACTION_CMD_RegisterNewService, ot::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCreateNewSession, SessionService, OT_ACTION_CMD_CreateNewSession, ot::ALL_MESSAGE_TYPES)

	OT_HANDLER(handleCheckProjectOpen, SessionService, OT_ACTION_CMD_IsProjectOpen, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSendBroadcastMessage, SessionService, OT_ACTION_CMD_SendBroadcastMessage, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleGetSessionExists, SessionService, OT_ACTION_CMD_GetSessionExists, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceClosing, SessionService, OT_ACTION_CMD_ServiceClosing, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceShutdownCompleted, SessionService, OT_ACTION_CMD_ServiceShutdownCompleted, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleMessage, SessionService, OT_ACTION_CMD_Message, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleShutdownSession, SessionService, OT_ACTION_CMD_ShutdownSession, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceFailure, SessionService, OT_ACTION_CMD_ServiceFailure, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleEnableServiceDebug, SessionService, OT_ACTION_CMD_ServiceEnableDebug, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleDisableServiceDebug, SessionService, OT_ACTION_CMD_ServiceDisableDebug, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleReset, SessionService, OT_ACTION_CMD_Reset, ot::SECURE_MESSAGE_TYPES) // todo: you thought i work? well.. no i do not (yet) :D
	OT_HANDLER(handleGetDebugInformation, SessionService, OT_ACTION_CMD_GetDebugInformation, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleCheckStartupCompleted, SessionService, OT_ACTION_CMD_CheckStartupCompleted, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceShow, SessionService, OT_ACTION_CMD_ServiceShow, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceHide, SessionService, OT_ACTION_CMD_ServiceHide, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleAddMandatoryService, SessionService, OT_ACTION_CMD_AddMandatoryService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleRegisterNewGlobalDirectoryService, SessionService, OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleServiceStartupFailed, SessionService, OT_ACTION_CMD_ServiceStartupFailed, ot::SECURE_MESSAGE_TYPES)

	OT_HANDLER(handleSetGlobalLogFlags, SessionService, OT_ACTION_CMD_SetGlobalLogFlags, ot::SECURE_MESSAGE_TYPES)

	void workerShutdownSession(ot::serviceID_t _serviceId, Session* _session);

	std::mutex                                  m_mutex;

	std::string									m_dataBaseURL;                      //! @brief Database IP address
	std::string									m_siteID;							//! @brief Site ID
	std::string									m_serviceAuthorisationURL;			//! @brief Authorization service IP address

	std::string									m_ip;
	std::string									m_port;
	ot::serviceID_t								m_id;
	ot::SystemInformation						m_systemLoadInformation;

	//NOTE, debug only this variable contains the IP address that is used for the services 
	
	std::map<std::string, bool>					m_serviceDebugList;
	std::map<std::string, std::vector<ot::ServiceBase> *> m_mandatoryServicesMap;	//! @brief Map containing all names of mandatory services for each session type
	std::map<std::string, Session *>			m_sessionMap;						//! @brief Map containing all sessions	

	

	std::list<std::string> m_sessionShutdownQueue;

	ot::LogModeManager m_logModeManager;

	SessionService(const SessionService&) = delete;
	SessionService & operator =(const SessionService&) = delete;
};
