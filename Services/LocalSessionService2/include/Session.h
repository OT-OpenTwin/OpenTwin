//! @file Session.h
//! @authors Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/PortManager.h"
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTServiceFoundation/UserCredentials.h"
#include "OTServiceFoundation/IDManager.h"

// std header
#include <list>
#include <mutex>
#include <string>
#include <optional>

class Session {
	OT_DECL_NOCOPY(Session)
	OT_DECL_NODEFAULT(Session)

	OT_DECL_DEFMOVE(Session)
public:
	enum SessionStateFlag {
		NoState      = 0 << 0, //! @brief No state set.
		ShuttingDown = 1 << 0  //! @brief Session is shutting down.
	};
	typedef ot::Flags<SessionStateFlag> SessionState; //! @brief Flags used to describe the state of the session.

	//! @brief Constructor.
	//! @param _id The ID of the session.
	//! @param _name The name of the session.
	Session(const std::string& _id, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _type);

	~Session();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	std::string getId() const { return m_id; };
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

	std::list<std::string> getToolBarTabOrder(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service management

	//! @brief Will store the provided service in this session.
	//! @param _service The service to store.
	Service& addService(Service&& _service);
	
	//! @brief Will remove the service with the specified ID from this session.
	//! @param _serviceID The ID of the service to remove.
	//! @param _notifyOthers If true, a broadcast message will be send that the service is shutting down.
	void serviceDisconnected(ot::serviceID_t _serviceID, bool _notifyOthers = false);

	//! @brief Will return the writeable reference to the service with the specified ID.
	//! @param _serviceID Service ID.
	//! @return Empty optional if the service is not found, otherwise reference to the specified service.
	std::optional<Service&> getServiceFromID(ot::serviceID_t _serviceID);

	//! @brief Will return the read only reference to the service with the specified ID.
	//! @param _serviceID Service ID.
	//! @return Empty optional if the service is not found, otherwise const reference to the specified service.
	std::optional<const Service&> getServiceFromID(ot::serviceID_t _serviceID) const;

	//! @brief Will return the writeable reference to the service with the specified url.
	//! @param _serviceURL Service url.
	//! @return Empty optional if the service is not found, otherwise reference to the specified service.
	std::optional<Service&> getServiceFromURL(const std::string& _serviceURL);

	//! @brief Will return the read only reference to the service with the specified url.
	//! @param _serviceURL Service url.
	//! @return Empty optional if the service is not found, otherwise const reference to the specified service.
	std::optional<const Service&> getServiceFromURL(const std::string& _serviceURL) const;

	//! @brief Will add the alive services to the provided JSON array.
	//! @param _array JSON array to add the services to.
	//! @param _allocator Allocator.
	void addAliveServicesToJsonArray(ot::JsonArray& _array, ot::JsonAllocator& _allocator) const;

	ot::serviceID_t generateNextServiceID() { return m_serviceIdManager.nextID(); };

	void startHealthCheck();
	void stopHealthCheck();

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

	// ###########################################################################################################################################################################################################################################################################################################################

	// Messaging

	//! @brief Broadcast a message action with the given message as a parameter to all alive services of this session that may receive broadcast messages.
	//! @param _senderService The sender of this message.
	//! @param _message The message text.
	void broadcastMessage(ot::serviceID_t _senderServiceID, const std::string& _message);

	//! @brief Broadcast a action to all alive services of this session that may receive broadcast messages.
	//! @param _senderService The sender of this message.
	//! @param _action The action to broadcast.
	void broadcastBasicAction(ot::serviceID_t _senderServiceID, const std::string& _action);

	//! @brief Will broadcast the provided message to all services in this session.
	//! @param _sender The sender of this message. The sender will not receive the broadcast message.
	//! @param _message The message to broadcast.
	//! @param _async If true the massage(s) will be send from a worker thread.
	void broadcast(ot::serviceID_t _senderServiceID, const std::string& _message, bool _async = false);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	void broadcastImpl(ot::serviceID_t _senderServiceID, const std::string& _message);

	//! @brief Will prepare the broadcast document with the sender information.
	//! @param _doc The document to prepare.
	//! @param _action The action that will be broadcasted.
	//! @param _senderService The ID of the service that is sending the broadcast message.
	void prepareBroadcastDocument(ot::JsonDocument& _doc, const std::string& _action, ot::serviceID_t _senderService) const;

	void prepareServiceFailure(Service& _failedService);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	void broadcastWorker(ot::serviceID_t _senderServiceID, std::string _message);

	void healthCheckWorker();

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
