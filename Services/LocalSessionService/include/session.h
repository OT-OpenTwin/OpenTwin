//! @file Session.h
//! @authors Alexander Kuester (alexk95)
//! @date December 2020
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/PortManager.h"
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTServiceFoundation/UserCredentials.h"
#include "OTServiceFoundation/IdManager.h"

// std header
#include <string>
#include <map>
#include <list>
#include <vector>

// Forward declarations
class Service;
class RelayService;
namespace std { class thread; }

//! @class Session
//! @brief The Session is used to hold all session related information required by the LSS.
class Session {
	OT_DECL_NOCOPY(Session)
	OT_DECL_NODEFAULT(Session)
public:
	//! @brief Constructor
	//! @param _id The ID of the session
	//! @param _name The name of the session
	Session(const std::string& _id, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _type);

	Session(Session&& _other) noexcept;

	virtual ~Session();

	Session& operator = (Session&& _other) noexcept;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Management

	//! @brief Will add the provided service information to the requested list
	//! This list is used to identify if all the requested services have been registered
	void addRequestedService(const std::string& _serviceName, const std::string& _serviceType);

	void removeRequestedService(const std::string& _serviceName, const std::string& _serviceType);

	//! @brief Returns the vector containing all still requested services
	const std::vector<std::pair<std::string, std::string>>& requestedServices(void) const { return m_requestedServices; };

	//! @brief Will store the provided service information and return the ID for the new service
	//! @param _serviceIP The IP of the new service
	//! @param _serviceName The name of the service
	//! @param _serviceType The type of the service
	Service& registerService(const std::string& _serviceUrl, const std::string& _serviceName, const std::string& _serviceType, ot::serviceID_t _serviceID);

	//! @brief Will store the provided service as service in this session.
	//! @param _service The service to store.
	Service& registerService(Service&& _service);
	
	//! @brief Will remove the service with the specified ID from this session
	//! @param _serviceID The ID of the service to remove
	//! @param _notifyOthers If true, a broadcast message will be send that the service is shutting down
	void removeService(ot::serviceID_t _serviceID, bool _notifyOthers = false);

	void addServiceListToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	//! @brief Will return a list of JSON objects containing information about each service
	void servicesInformation(ot::JsonArray& _servicesInfo, ot::JsonAllocator& _allocator) const;

	//! @brief Will send a broadcast message to all services of this session
	//! @param _sender The sender of this message
	//! @param _message The message text
	//! @param _async If true the massage(s) will be send from a worker thread
	void broadcastMessage(const std::string& _senderServiceUrl, const std::string& _message, bool _async = false);

	//! @brief Will send a broadcast message to all services of this session
	//! @param _sender The sender of this message
	//! @param _command The command to send
	//! @param _async If true the massage(s) will be send from a worker thread
	void broadcastAction(const std::string& _senderServiceUrl, const std::string& _command, bool _async = false);

	//! @brief Will broadcast the provided message to all services in this session
	//! @param _sender The sender of this message
	//! @param _message The message
	//! @param _async If true the massage(s) will be send from a worker thread
	void broadcast(const std::string& _senderServiceUrl, const ot::JsonDocument& _message, bool _shutdown, bool _async = false);

	//! @brief Will shutdown the session.
	//! The source of the shutdown is the specified service
	//! and therefore the service will not receive any shutdown related messages.
	void shutdownByService(const std::string& _serviceUrl);

	void serviceFailure(const std::string& _serviceUrl);

	//! @brief Adds the proided port number to the debug port list.
	//! Debug ports will be freed when the session is closing.
	void addDebugPortInUse(ot::port_t _port);

	bool hasService(ot::serviceID_t _serviceId) const;

	//! @brief Will return the service with the specified ID.
	//! @param _serviceIP The ID of the service.
	//! @throw ObjectNotFound exception if the ID is invalid.
	Service& findService(ot::serviceID_t _serviceID);

	//! @brief Will return the service with the specified URL.
	//! @param _serviceURL The URL of the service.
	//! @throw ObjectNotFound exception if the URL is invalid.
	Service& findServiceFromURL(const std::string& _serviceURL);

	Service& findServiceFromNameType(const std::string& _serviceName, const std::string& _serviceType);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	std::list<std::string> getToolBarTabOrder(void);

	std::string getId(void) const { return m_id; };
	std::string getUserName(void) const { return m_userName; };
	std::string getProjectName(void) const { return m_projectName; };
	std::string getCollectionName(void) const { return m_collectionName; };
	std::string getType(void) const { return m_type; };
	size_t getServiceCount(void) const { return m_serviceMap.size(); };

	void setCredentialsUsername(const std::string &username) { m_userCredentials.setUserName(username); };
	void setCredentialsPassword(const std::string &password) { m_userCredentials.setEncryptedPassword(password); };
	void setUserCollection(const std::string &collection)    { m_userCollection = collection; }

	void setDatabaseUsername(const std::string& username) { m_dbCredentials.setUserName(username); };
	void setDatabasePassword(const std::string& password) { m_dbCredentials.setEncryptedPassword(password); };

	std::string getCredentialsUsername(void) const { return m_userCredentials.getUserName(); }
	std::string getCredentialsPassword(void) const { return m_userCredentials.getEncryptedPassword(); }
	std::string getUserCollection(void) const { return m_userCollection; }
	std::string getType(void) const { return m_type; }

	std::string getDatabaseUsername(void) const { return m_dbCredentials.getUserName(); }
	std::string getDatabasePassword(void) const { return m_dbCredentials.getEncryptedPassword(); }

	ot::serviceID_t generateNextServiceId(void);

private:
	std::string										 m_id;
	std::string										 m_userName;
	std::string										 m_projectName;
	std::string										 m_collectionName;
	std::string										 m_type;
	
	ot::UserCredentials								 m_userCredentials;
	ot::UserCredentials								 m_dbCredentials;
	std::string										 m_userCollection;

	std::map<ot::serviceID_t, Service>               m_serviceMap;
	ot::IDManager<ot::serviceID_t>					 m_serviceIdManager;
	std::vector<std::pair<std::string, std::string>> m_requestedServices; //! first = name, second = type
	std::list<ot::port_t>							 m_debugPorts;
};
