// @otlicense

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

class SessionService : public ot::ActionHandler {
	OT_DECL_NOCOPY(SessionService)
	OT_DECL_NOMOVE(SessionService)
	OT_DECL_ACTION_HANDLER(SessionService)
private:
	SessionService();
	~SessionService();
public:
	static SessionService& instance();
	static int initialize(const std::string& _ownUrl, const std::string& _gssUrl);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	const std::string& getUrl() const { return m_url; };

	const std::string& getAuthorisationServiceUrl() const { return m_authUrl; };

	const std::string& getDatabaseUrl() const { return m_dataBaseUrl; };

	const std::string& getSiteID() const { return m_siteID; };

	const std::string& getLMSUrl() const { return m_lmsUrl; };

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

	void startWorkerThreads();
	void stopWorkerThreads();
	void shutdownWorker();
	bool checkShuttingDown();
	bool checkShutdownCompleted();
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler

	ot::ReturnMessage handleGetAuthURL();
	ot::ReturnMessage handleGetGlobalServicesURL(ot::JsonDocument& _commandDoc);
	ot::ReturnMessage handleCreateNewSession(ot::JsonDocument& _commandDoc);

	std::string handleGetSystemInformation();
	std::string handleGetMandatoryServices();
	ot::ReturnMessage handleConfirmService(ot::JsonDocument& _commandDoc);
	ot::ReturnMessage handleShowService(ot::JsonDocument& _commandDoc);
	ot::ReturnMessage handleGetSessionExists(ot::JsonDocument& _commandDoc);
	void handleServiceClosing(ot::JsonDocument& _commandDoc);
	ot::ReturnMessage handleShutdownSession(ot::JsonDocument& _commandDoc);
	void handleServiceFailure(ot::JsonDocument& _commandDoc);
	void handleServiceShutdownCompleted(ot::JsonDocument& _commandDoc);
	ot::ReturnMessage handleEnableServiceDebug(ot::JsonDocument& _commandDoc);
	ot::ReturnMessage handleDisableServiceDebug(ot::JsonDocument& _commandDoc);
	std::string handleGetDebugInformation();
	ot::ReturnMessage handleCheckStartupCompleted(ot::JsonDocument& _commandDoc);
	ot::ReturnMessage handleAddMandatoryService(ot::JsonDocument& _commandDoc);
	ot::ReturnMessage handleRegisterNewGlobalDirectoryService(ot::JsonDocument& _commandDoc);
	ot::ReturnMessage handleRegisterNewLibraryManagementService(ot::JsonDocument& _commandDoc);
	ot::ReturnMessage handleServiceStartupFailed(ot::JsonDocument& _commandDoc);
	void handleSetGlobalLogFlags(ot::JsonDocument& _commandDoc);

	std::mutex                                        m_mutex;
	std::atomic_bool                                  m_workerRunning;
	std::thread*                                      m_shutdownWorkerThread;

	GlobalSessionService                              m_gss;
	GlobalDirectoryService                            m_gds;

	std::string                                       m_authUrl;
	std::string                                       m_dataBaseUrl;
	std::string                                       m_siteID; //! @brief Site ID
	std::string                                       m_lmsUrl;

	std::string                                       m_url;
	ot::serviceID_t                                   m_id;
	ot::SystemInformation                             m_systemLoadInformation;

	std::unordered_set<std::string>                   m_debugServices;

	std::map<std::string, Session>                    m_sessions;
	std::map<std::string, std::list<ot::ServiceBase>> m_mandatoryServicesMap; //! @brief Map containing all names of mandatory services for each session type

	std::list<std::pair<std::string, bool>>           m_shutdownQueue; //! @brief List of sessions that are currently shutting down, second = emergency shutdown
	std::list<std::string>                            m_shutdownCompletedQueue;

	ot::PortManager                                   m_debugPortManager;
};
