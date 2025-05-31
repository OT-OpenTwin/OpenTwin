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
#include <string>
#include <optional>

class Session {
	OT_DECL_NOCOPY(Session)
	OT_DECL_NODEFAULT(Session)

	OT_DECL_DEFMOVE(Session)
public:
	//! @brief Constructor
	//! @param _id The ID of the session
	//! @param _name The name of the session
	Session(const std::string& _id, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _type);

	virtual ~Session();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	std::string getId(void) const { return m_id; };
	std::string getUserName(void) const { return m_userName; };
	std::string getProjectName(void) const { return m_projectName; };
	std::string getCollectionName(void) const { return m_collectionName; };
	std::string getType(void) const { return m_type; };
	size_t getServiceCount(void) const { return m_services.size(); };

	void setCredentialsUsername(const std::string& username) { m_userCredentials.setUserName(username); };
	void setCredentialsPassword(const std::string& password) { m_userCredentials.setEncryptedPassword(password); };
	void setUserCollection(const std::string& collection) { m_userCollection = collection; }

	void setDatabaseUsername(const std::string& username) { m_dbCredentials.setUserName(username); };
	void setDatabasePassword(const std::string& password) { m_dbCredentials.setEncryptedPassword(password); };

	std::string getCredentialsUsername(void) { return m_userCredentials.getUserName(); }
	std::string getCredentialsPassword(void) { return m_userCredentials.getEncryptedPassword(); }
	std::string getUserCollection(void) { return m_userCollection; }
	std::string getType(void) { return m_type; }

	std::string getDatabaseUsername(void) { return m_dbCredentials.getUserName(); }
	std::string getDatabasePassword(void) { return m_dbCredentials.getEncryptedPassword(); }

	std::list<std::string> getToolBarTabOrder(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Service management

	//! @brief Will store the provided service as service in this session
	//! @param _service The service to store
	Service& registerService(Service&& _service);
	
	//! @brief Will remove the service with the specified ID from this session
	//! @param _serviceID The ID of the service to remove
	//! @param _notifyOthers If true, a broadcast message will be send that the service is shutting down
	void removeService(ot::serviceID_t _serviceID, bool _notifyOthers = false);

	//! @brief Will return the service with the specified ID
	//! @param _serviceIP The ID of the service
	std::optional<Service&> getServiceFromId(ot::serviceID_t _serviceID) const;

	//! @brief Will return the service with the specified URL
	//! @param _serviceURL The URL of the service
	std::optional<Service&> getServiceFromURL(const std::string& _serviceURL) const;

	void addVisibleServicesToJsonArray(ot::JsonArray& _array, ot::JsonAllocator& _allocator) const;

	//! @brief Will return a list of JSON objects containing information about each service
	void servicesInformation(ot::JsonArray& servicesInfo, ot::JsonAllocator &allocator) const;

	//! @brief Broadcast a message action with the given message to all visible services of this session that may receive broadcast messages.
	//! @param _senderService The sender of this message.
	//! @param _message The message text.
	//! @param _async If true the massage will be send from a worker thread.
	void broadcastMessage(ot::serviceID_t _senderServiceID, const std::string& _message);

	//! @brief Will broadcast the provided message to all services in this session
	//! @param _sender The sender of this message
	//! @param _message The message
	//! @param _async If true the massage(s) will be send from a worker thread
	void broadcast(ot::serviceID_t _senderServiceID, const ot::JsonDocument& _message, bool _shutdown, bool _async = false);

	//! @brief Will shutdown the session
	void shutdown(ot::serviceID_t _senderServiceID);

	void serviceFailure(ot::serviceID_t _serviceID);

	ot::serviceID_t generateNextServiceId(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	//! @brief Will prepare the broadcast document with the sender information.
	//! @param _doc The document to prepare.
	//! @param _action The action that will be broadcasted.
	//! @param _senderService The ID of the service that is sending the broadcast message.
	void prepareBroadcastDocument(ot::JsonDocument& _doc, const std::string& _action, ot::serviceID_t _senderService) const;

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
