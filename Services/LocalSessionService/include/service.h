/*
 * service.h
 *
 *  Created on: November 30, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

// SessionService header
#include <globalDatatypes.h>

#include "OTCore/rJSON.h"
#include "OTCore/CoreTypes.h"

// C++ header
#include <string>
#include <list>

class Session;

class Service {
public:

	//! @brief Constructor
	//! @param _url The URL of this service
	//! @param _name The name of this service
	//! @param _id The ID of this service
	//! @param _session The session this service belongs to
	Service(
		const std::string &			_url,
		const std::string &			_name,
		ot::serviceID_t				_id,
		const std::string &			_type,
		Session *					_session,
		bool						_showDebugInfo
	);

	//! @brief Deconstructor
	virtual ~Service() {}

	// ######################################################################################

	// Setter

	//! @brief Will set the ip of this service
	//! @param _ip The IP to set
	void setURL(
		const std::string &			_url
	) { m_url = _url; }

	//! @brief Will set the name of this service
	//! @param _name The name to set
	void setName(
		const std::string &			_name
	) { m_name = _name; }

	//! @brief Will set the ID for this service
	//! @param _id The ID to set
	void setID(
		ot::serviceID_t					_id
	) { m_id = _id; }

	//! @brief Will set the receive broadcast messages flag
	void setReceiveBroadcastMessages(
		bool						_receive
	) { m_receiveBroadcastMessages = _receive; }

	//! @brief Will set the visible state of the service
	//! @param _isVisible if true the service will appear as visible
	void setVisible(void);

	//! @brief Will set the visible state of the service to false and notify all other services in the same session
	void setHidden(void);

	// ######################################################################################

	// Getter

	//! @brief Will return the ip of this service
	std::string url(void) const { return m_url; }

	//! @brief Will return the port numbers of this service
	virtual std::list<unsigned long long> portNumbers(void) const;

	//! @brief Will return the name of this service
	std::string name(void) const { return m_name; }

	//! @brief Will return the ID of this service
	ot::serviceID_t id(void) const { return m_id; }

	//! @brief Will return the type of this service
	std::string type(void) const { return m_type; }

	//! @brief Will return the session this service belongs to
	Session * getSession(void) const { return m_session; }

	//! @brief Will return true if this service wants to receive broadcast messages
	bool receiveBroadcastMessages(void) const { return m_receiveBroadcastMessages; }

	//! @brief Will return true if this service is visible
	bool isVisible(void) const { return m_isVisible; }

	//! @brief Will return a JSON string representing this service
	virtual std::string toJSON(void) const;

	void writeDataToValue(OT_rJSON_doc & _doc, OT_rJSON_val & _value);

protected:

	std::string				m_url;			//! This services IP address
	std::string				m_name;			//! This services name
	ot::serviceID_t			m_id;			//! This services ID
	std::string				m_type;			//! This services type
	Session *				m_session;		//! The session this service belongs to
	bool					m_receiveBroadcastMessages;
	bool					m_isVisible;

private:

	Service() = delete;
	Service(const Service &) = delete;
};
