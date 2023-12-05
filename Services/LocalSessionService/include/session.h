/*
 * session.h
 *
 *  Created on: December 03, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

// C++ header
#include <string>
#include <map>
#include <list>
#include <vector>
#include <mutex>

// SessionService header
#include <globalDatatypes.h>
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTSystem/PortManager.h"
#include "OTServiceFoundation/UserCredentials.h"
#include "OTServiceFoundation/IdManager.h"

class Service;
class RelayService;

namespace std { class thread; }

class Session {
public:

	//! @brief Constructor
	//! @param _id The ID of the session
	//! @param _name The name of the session
	Session(const std::string& _ID, const std::string& _userName, const std::string& _projectName, const std::string& _collectionName, const std::string& _type);

	virtual ~Session();

	// ####################################################################################################

	// Information getter

	//! @brief Will return the sessions ID
	std::string id(void) const { return m_id; }

	//! @brief Will return the user name
	std::string userName(void) const { return m_userName; }

	//! @brief Will return the project name
	std::string projectName(void) const { return m_projectName; }

	//! @brief WIll return the collection name
	std::string collectionName(void) const { return m_collectionName; }

	//! @brief Will return the type
	std::string type(void) const { return m_type; }

	//! @brief Will return the count of the services registered in this session
	size_t serviceCount() const { return m_serviceMap.size(); }

	//! @brief Will return true if the session is in debug mode
	bool isDebug(void) const { return m_sessionDebug; }

	//! @brief Will set the session debug mode
	void setIsDebug(bool _debug = true) { m_sessionDebug = _debug; }

	// ####################################################################################################

	// Service management

	//! @brief Will add the provided service information to the requested list
	//! This list is used to identify if all the requested services have been registered
	void addRequestedService(const std::string& _serviceName, const std::string& _serviceType);

	void removeRequestedService(const std::string& _serviceName, const std::string& _serviceType);

	//! @brief Returns the vector containing all still requested services
	const std::vector<std::pair<std::string, std::string>>& requestedServices(void) const { return m_requestedServices; };

	//! @brief Adds the proided port number to the debug port list
	//! Debug ports will be freed when the session is closing
	void addDebugPortInUse(ot::port_t _port);

	//! @brief Will store the provided service information and return the ID for the new service
	//! @param _serviceIP The IP of the new service
	//! @param _serviceName The name of the service
	//! @param _serviceType The type of the service
	Service * registerService(const std::string& _serviceIP, const std::string& _serviceName, const std::string& _serviceType, ot::serviceID_t _serviceID);

	//! @brief Will store the provided service as service in this session
	//! @param _service The service to store
	Service * registerService(Service * _service);
	
	//! @brief Will remove the service with the specified ID from this session
	//! @param _serviceID The ID of the service to remove
	//! @param _notifyOthers If true, a broadcast message will be send that the service is shutting down
	void removeService(ot::serviceID_t _serviceID, bool _notifyOthers = false);

	//! @brief Will return the service with the specified ID
	//! @param _serviceIP The ID of the service
	Service * getService(ot::serviceID_t _serviceID);

	//! @brief Will return the service with the specified URL
	//! @param _serviceURL The URL of the service
	Service * getServiceFromURL(const std::string& _serviceURL);

	//! @brief Will return all services with the given name in this session
	std::list<Service *> getServicesByName(const std::string& _serviceName);

	//! @brief Will return a JSON string containing all services in this session
	std::string getServiceListJSON(void);

	//! @brief Will return a JSON string representing the main information about this session
	std::string infoToJSON(void) const;

	//! @brief Will add all services to the provided document
	void addServiceListToDocument(ot::JsonDocument& _doc);

	//! @brief Will send a broadcast message to all services of this session
	//! @param _sender The sender of this message
	//! @param _message The message text
	//! @param _async If true the massage(s) will be send from a worker thread
	void broadcastMessage(Service * _sender, const std::string& _message, bool _async = false);

	//! @brief Will send a broadcast message to all services of this session
	//! @param _sender The sender of this message
	//! @param _command The command to send
	//! @param _async If true the massage(s) will be send from a worker thread
	void broadcastAction(Service * _sender, const std::string& _command, bool _async = false);

	//! @brief Will broadcast the provided message to all services in this session
	//! @param _sender The sender of this message
	//! @param _message The message
	//! @param _async If true the massage(s) will be send from a worker thread
	void broadcast(
		Service *					_sender,
		const ot::JsonDocument&		_message,
		bool						_shutdown,
		bool                        _async = false
	);

	//! @brief Will shutdown the session
	void shutdown(
		Service *					_sender
	);

	void serviceFailure(
		Service *					_failedService
	);

	std::list<std::string> toolBarTabOrder(void);

	void setCredentialsUsername(const std::string &username) { m_userCredentials.setUserName(username); };
	void setCredentialsPassword(const std::string &password) { m_userCredentials.setEncryptedPassword(password); };
	void setUserCollection(const std::string &collection)    { m_userCollection = collection; }

	std::string getCredentialsUsername(void) { return m_userCredentials.getUserName(); }
	std::string getCredentialsPassword(void) { return m_userCredentials.getEncryptedPassword(); }
	std::string getUserCollection(void) { return m_userCollection; }

	ot::serviceID_t generateNextServiceId(void);

private:

	std::mutex											m_mutex;

	std::string											m_id;
	std::string											m_userName;
	std::string											m_projectName;
	std::string											m_collectionName;
	std::string											m_type;
	bool												m_sessionDebug;

	ot::UserCredentials									m_userCredentials;
	std::string											m_userCollection;

	std::map<ot::serviceID_t, Service *>				m_serviceMap;
	ot::IDManager<ot::serviceID_t>						m_serviceIdManager;
	std::vector<std::pair<std::string, std::string>>	m_requestedServices; //! first = name, second = type
	std::list<ot::port_t>								m_debugPorts;

	Session() = delete;
	Session(Session &) = delete;
	Session & operator = (Session &) = delete;

};
