/*
 * SessionService.h
 *
 *  Created on: November 26, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

// SessionService header
#include "globalDatatypes.h"
#include "PortManager.h"
#include "Service.h"
#include "GlobalDirectoryService.h"

#include "OTCore/ServiceBase.h"
#include "OTCore/OTObjectBase.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTSystem/SystemLoadInformation.h"

// C++ header
#include <string>
#include <map>
#include <vector>
#include <mutex>

class Session;
class RelayService;
class GlobalSessionService;

class SessionService : public ot::OTObjectBase {
public:
	SessionService();
	virtual ~SessionService() {}; // todo: add cleanup
	
	static SessionService * instance(void);

	// ######################################################################################

	// Setter / Getter

	void setIp(const std::string& _ip) { m_ip = _ip; };
	const std::string& ip(void) const { return m_ip; };

	void setPort(const std::string& _port);
	const std::string& port(void) const { return m_port; };

	void setId(ot::serviceID_t _id) { m_id = _id; };
	ot::serviceID_t id(void) const { return m_id; };

	std::string url(void) const { return m_ip + ":" + m_port; };

	//! @brief Will set the database IP
	//! @param _ip The IP to set
	void setDataBaseURL(const std::string& _url) { m_dataBaseURL = _url; }

	//! @brief Will return the database IP
	const std::string& dataBaseURL(void) const { return m_dataBaseURL; }

	//! @brief Will set the site ID
	//! @param _ID The ID to set
	void setSiteID(std::string _ID) { m_siteID = _ID; }

	//! @brief Will return the session service site ID
	const std::string& siteID(void) const { return m_siteID; }

	//! @brief Will set the authorisation service IP
	//! @param _ip The IP to set
	void setAuthorisationServiceURL(const std::string& _url) { m_serviceAuthorisationURL = _url; }

	//! @brief Will return the session service service directory IP
	const std::string& serviceAuthorisationURL(void) const { return m_serviceAuthorisationURL; }

	void setGlobalDirectoryServiceURL(const std::string& _url);

	//! @brief Will return true if the provided service is in debug mode
	bool isServiceInDebugMode(const std::string& _serviceName);

	// ######################################################################################

	// Getter
	
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
	Session * createSession(
		const std::string &						_sessionID,
		const std::string &						_userName,
		const std::string &						_projectName,
		const std::string &						_collectionName,
		const std::string &						_sessionType
	);

	//! @brief Returns the session with the specified id
	//! @param _sessionID The ID of the session to get
	//! @param _throwException If true, exceptions will be thrown, otherwise a nullptr will be returned
	//! @throw std::exception if the specified session ID is invalid
	Session * getSession(
		const std::string &						_sessionID,
		bool									_throwException = true
	);

	//! @brief Will ensure that all mandatory services for the specified session are running
	//! @param _sessionID The ID of the session
	bool runMandatoryServices(
		const std::string &						_sessionID
	);

	//! @brief Will ensure that all mandatory services for the specified session are running
	//! @param _session The session
	bool runMandatoryServices(
		Session *								_session
	);

	//! @brief Will close the service and deregister it from its session
	void serviceClosing(
		Service *								_service,
		bool									_notifyOthers,
		bool									_autoCloseSessionIfMandatory = true
	);

	//! @brief Will remove the session and close its logger.
	//! Will NOT perform the session shutdown().
	//! Will clear the memory of the provided pointer.
	//! @param _session The session to remove
	void removeSession(
		Session *								_session
	);

	std::list<const Session *> sessions(void);
	
	bool runServiceInDebug(const std::string& _serviceName, const std::string& _serviceType, Session* _session, std::string& _serviceURL);
	
	bool runRelayService(Session * _session, std::string& _websocketURL, std::string& _serviceURL);

	void setGlobalSessionService(GlobalSessionService * _gss) { m_globalSessionService = _gss; }

	Service * getServiceFromURL(const std::string& _url);

	//! @brief Initialize the collection of system information (e.g. CPU time and memory).
	void initializeSystemInformation();

private:
	// Message handling

	OT_HANDLER(handleGetDBURL, SessionService, OT_ACTION_CMD_GetDatabaseUrl, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleGetAuthURL, SessionService, OT_ACTION_CMD_GetAuthorisationServerUrl, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleGetGlobalServicesURL, SessionService, OT_ACTION_CMD_GetGlobalServicesUrl, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleGetProjectTypes, SessionService, OT_ACTION_CMD_GetListOfProjectTypes, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleGetSystemInformation, SessionService, OT_ACTION_CMD_GetSystemInformation, ot::ALL_MESSAGE_TYPES);

	OT_HANDLER(handleGetMandatoryServices, SessionService, OT_ACTION_CMD_GetMandatoryServices, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleSessionDebugLogOn, SessionService, OT_ACTION_CMD_SessionDebugLogOn, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleSessionDebugLogOff, SessionService, OT_ACTION_CMD_SessionDebugLogOff, ot::SECURE_MESSAGE_TYPES);

	OT_HANDLER(handleRegisterNewService, SessionService, OT_ACTION_CMD_RegisterNewService, ot::ALL_MESSAGE_TYPES);
	OT_HANDLER(handleCreateNewSession, SessionService, OT_ACTION_CMD_CreateNewSession, ot::ALL_MESSAGE_TYPES);

	OT_HANDLER(handleCheckProjectOpen, SessionService, OT_ACTION_CMD_IsProjectOpen, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleSendBroadcastMessage, SessionService, OT_ACTION_CMD_SendBroadcastMessage, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleGetServicesInSession, SessionService, OT_ACTION_CMD_GetSessionServices, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleGetSessionExists, SessionService, OT_ACTION_CMD_GetSessionExists, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleServiceClosing, SessionService, OT_ACTION_CMD_ServiceClosing, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleMessage, SessionService, OT_ACTION_CMD_Message, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleShutdownSession, SessionService, OT_ACTION_CMD_ShutdownSession, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleServiceFailure, SessionService, OT_ACTION_CMD_ServiceFailure, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleEnableServiceDebug, SessionService, OT_ACTION_CMD_ServiceEnableDebug, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleDisableServiceDebug, SessionService, OT_ACTION_CMD_ServiceDisableDebug, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleReset, SessionService, OT_ACTION_CMD_Reset, ot::SECURE_MESSAGE_TYPES); // todo: you thought i work? well.. no i do not (yet) :D
	OT_HANDLER(handleGetDebugInformation, SessionService, OT_ACTION_CMD_GetDebugInformation, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleCheckStartupCompleted, SessionService, OT_ACTION_CMD_CheckStartupCompleted, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleServiceShow, SessionService, OT_ACTION_CMD_ServiceShow, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleServiceHide, SessionService, OT_ACTION_CMD_ServiceHide, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleAddMandatoryService, SessionService, OT_ACTION_CMD_AddMandatoryService, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleRegisterNewGlobalDirectoryService, SessionService, OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleServiceStartupFailed, SessionService, OT_ACTION_CMD_ServiceStartupFailed, ot::SECURE_MESSAGE_TYPES);

	void workerShutdownSession(ot::serviceID_t _serviceId, std::string _sessionId);

	std::string									m_dataBaseURL;						//! The database IP address
	std::string									m_siteID;							//! The site ID
	std::string									m_serviceAuthorisationURL;			//! The authorization service IP address

	std::mutex									m_masterLock;

	std::string									m_ip;
	std::string									m_port;
	ot::serviceID_t								m_id;
	ot::SystemLoadInformation					m_systemLoadInformation;

	//NOTE, debug only this variable contains the IP address that is used for the services 
	
	std::map<std::string, bool>					m_serviceDebugList;
	std::map<std::string,
		std::vector<ot::ServiceBase> *>			m_mandatoryServicesMap;				//! Map containing all names of mandatory services for each session type
	std::map<std::string, Session *>			m_sessionMap;						//! Map containing all sessions	

	GlobalSessionService *						m_globalSessionService;
	GlobalDirectoryService *					m_globalDirectoryService;

	SessionService(const SessionService&) = delete;
	SessionService & operator =(const SessionService&) = delete;
};
