//! @file Session.h
//! @authors Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LSS header
#include "Service.h"

// OpenTwin header
#include "OTSystem/PortManager.h"
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ServiceRunData.h"
#include "OTCommunication/ServiceInitData.h"
#include "OTServiceFoundation/UserCredentials.h"
#include "OTServiceFoundation/IDManager.h"

// std header
#include <list>
#include <mutex>
#include <string>

class Session {
	OT_DECL_NOCOPY(Session)
	OT_DECL_NODEFAULT(Session)
public:
	enum SessionStateFlag {
		NoState      = 0 << 0, //! @brief No state set.
		ShuttingDown = 1 << 0  //! @brief Session is shutting down.
	};
	typedef ot::Flags<SessionStateFlag> SessionState; //! @brief Flags used to describe the state of the session.

	//! @brief Constructor.
	//! @param _id The ID of the session.
	//! @param _name The name of the session.
	explicit Session(const std::string& _id, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _type);

	Session(Session&& _other) noexcept;

	~Session();

	Session& operator=(Session&& _other) noexcept;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	std::string getID() const { return m_id; };
	std::string getUserName() const { return m_userName; };
	std::string getProjectName() const { return m_projectName; };
	std::string getCollectionName() const { return m_collectionName; };
	std::string getType() const { return m_type; };
	size_t getServiceCount() const { return m_services.size(); };

	void setCredentialsUsername(const std::string& username) { m_userCredentials.setUserName(username); };
	std::string getCredentialsUsername() const { return m_userCredentials.getUserName(); };

	void setCredentialsPassword(const std::string& password) { m_userCredentials.setEncryptedPassword(password); };
	std::string getCredentialsPassword() const { return m_userCredentials.getEncryptedPassword(); };

	void setUserCollection(const std::string& collection) { m_userCollection = collection; }
	std::string getUserCollection() const { return m_userCollection; };

	void setDatabaseUsername(const std::string& username) { m_dbCredentials.setUserName(username); };
	std::string getDatabaseUsername() const { return m_dbCredentials.getUserName(); };

	void setDatabasePassword(const std::string& password) { m_dbCredentials.setEncryptedPassword(password); };
	std::string getDatabasePassword() const { return m_dbCredentials.getEncryptedPassword(); };

	std::list<std::string> getToolBarTabOrder();

	//! @brief Returns true if any of the services in this session is flagged as requested.
	//! @param _ignoredService If provided, this service will be ignored in the check.
	bool hasRequestedServices(ot::serviceID_t _ignoredService);

	//! @brief Returns true if any of the services in this session is alive.
	bool hasAliveServices();

	//! @brief Returns true if any of the services in this session is shutting down.
	bool hasShuttingDownServices();

	bool isShuttingDown() const { return m_state.flagIsSet(Session::ShuttingDown); };

	ot::ServiceInitData createServiceInitData(ot::serviceID_t _serviceID);

	ot::ServiceRunData createServiceRunData(ot::serviceID_t _serviceID);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service management

	Service& addRequestedService(const ot::ServiceBase& _serviceInformation);
	
	void setServiceAlive(ot::serviceID_t _serviceID, bool _notifyOthers);

	void setServiceAlive(ot::serviceID_t _serviceID, const std::string& _serviceUrl, bool _notifyOthers);

	void showService(ot::serviceID_t _serviceID);

	ot::ServiceBase getServiceInfo(ot::serviceID_t _serviceID);

	void setServiceShutdownCompleted(ot::serviceID_t _serviceID);

	void serviceDisconnected(ot::serviceID_t _serviceID, bool _notifyOthers);

	void addAliveServicesToJsonArray(ot::JsonArray& _array, ot::JsonAllocator& _allocator);

	void startHealthCheck();
	void stopHealthCheck();

	void sendRunCommand();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Session management

	bool isSessionShuttingDown() const { return m_state & Session::ShuttingDown; };

	//! @brief Will prepare the session for shutdown.
	//! The session will be flagged as shutting down and the health check will be stopped.
	void prepareSessionForShutdown();

	//! @brief Will shutdown the session.
	//! On a regular shutdown a pre shutdown broadcast message will be sent.
	//! After the pre shutdown a shutdown broadcast message will be sent.
	//! The sender service will be remove from the session and all other services will be flagged as shutting down.
	//! 
	//! In case of a emergency shutdown the pre shutdown message will not be send and instead of the regular shutdown message a emergency shutdown broadcast message will be sent.
	//! @param _senderServiceID The ID of the service that initiated the shutdown.
	//! @param _emergencyShutdown If true, the session will be shutdown in emergency mode, otherwise a regular shutdown will be performed.
	void shutdownSession(ot::serviceID_t _senderServiceID, bool _emergencyShutdown);

	//! @brief Will broadcast the provided message to all services in this session.
	//! @param _sender The sender of this message. The sender will not receive the broadcast message.
	//! @param _message The message to broadcast.
	void sendBroadcast(ot::serviceID_t _senderServiceID, const std::string& _message);

	void removeFailedService(ot::serviceID_t _failedServiceID);

private:

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Messaging

	//! @brief Broadcast a action to all alive services of this session that may receive broadcast messages.
	//! @warning The mutex must be locked before calling this function.
	//! @param _senderService The sender of this message.
	//! @param _action The action to broadcast.
	void broadcastBasicAction(ot::serviceID_t _senderServiceID, const std::string& _action, bool _forceSend);

	//! @brief Will broadcast the provided message to all services in this session.
	//! @param _sender The sender of this message. The sender will not receive the broadcast message.
	//! @param _message The message to broadcast.
	//! @param _async If true the massage(s) will be send from a worker thread.
	//! @warning The mutex must be locked before calling this with the _async param set to false.
	void broadcast(ot::serviceID_t _senderServiceID, const std::string& _message, bool _async, bool _forceSend);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	//! @brief Will generate a new service ID.
	ot::serviceID_t generateNextServiceID() { return m_serviceIdManager.nextID(); };

	//! @brief Will store the provided service in this session.
	//! @param _service The service to store.
	Service& addService(Service&& _service);

	bool hasService(ot::serviceID_t _serviceID) const;

	//! @brief Will return the writeable reference to the service with the specified ID.
	//! @warning The mutex must be locked before calling this function.
	//! @param _serviceID Service ID.
	//! @return Empty optional if the service is not found, otherwise reference to the specified service.
	Service& getServiceFromID(ot::serviceID_t _serviceID);

	//! @brief Will return the read only reference to the service with the specified ID.
	//! @warning The mutex must be locked before calling this function.
	//! @param _serviceID Service ID.
	//! @return Empty optional if the service is not found, otherwise const reference to the specified service.
	const Service& getServiceFromID(ot::serviceID_t _serviceID) const;

	void broadcastImpl(ot::serviceID_t _senderServiceID, const std::string& _message, bool _forceSend);

	//! @brief Will prepare the broadcast document with the sender information.
	//! @warning The mutex must be locked before calling this function.
	//! @param _doc The document to prepare.
	//! @param _action The action that will be broadcasted.
	//! @param _senderService The ID of the service that is sending the broadcast message.
	void prepareBroadcastDocument(ot::JsonDocument& _doc, const std::string& _action, ot::serviceID_t _senderService);

	ot::ServiceRunData createServiceRunDataImpl(ot::serviceID_t _serviceID);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	void broadcastWorker(ot::serviceID_t _senderServiceID, std::string _message, bool _forceSend);

	void healthCheckWorker();

	void runWorker();

	void showWorker(ot::serviceID_t _visibleService);

	static void healthCheckFailedNotifier(std::string _sessionID, ot::serviceID_t _failedServiceID);

	std::mutex  			       m_mutex;
	std::thread				       m_healthCheckThread; //! @brief Thread for health checks.
	std::atomic_bool			   m_healthCheckRunning; //! @brief Flag to indicate if the health check thread is running.

	SessionState				   m_state;

	std::string                    m_id;
	std::string                    m_userName;
	std::string                    m_projectName;
	std::string                    m_collectionName;
	std::string                    m_type;
	
	ot::UserCredentials            m_userCredentials;
	ot::UserCredentials            m_dbCredentials;
	std::string                    m_userCollection;

	std::list<Service>             m_services;
	ot::IDManager<ot::serviceID_t> m_serviceIdManager;
};

OT_ADD_FLAG_FUNCTIONS(Session::SessionStateFlag)