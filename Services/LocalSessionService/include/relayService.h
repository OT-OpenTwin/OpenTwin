/*
 * relayService.h
 *
 *  Created on: January 08, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

#include "OTCore/CoreTypes.h"

// Session service header
#include <Service.h>

// C++ header
#include <string>

class RelayService : public Service {
public:

	//! @brief Constructor
	//! @param _ip The IP address of this service
	//! @param _name The name of this service
	//! @param _id The ID of this service
	//! @param _session The session this service belongs to
	//! @param _websocketIP The ip of the websocket
	//! @param _connectedService The service that is connected to the websocket
	RelayService(
		const std::string &			_ip,
		const std::string &			_name,
		ot::serviceID_t				_id,
		const std::string &			_type,
		Session *					_session,
		const std::string &			_websocketIp,
		Service *					_connectedService
	);

	//! @brief Deconstructor
	virtual ~RelayService();

	//! @brief Will return the connected service
	Service * connectedService() const { return m_connectedService; }

	//! @brief Will set the websocket ip of the relay service
	void setWebsocketIP(const std::string & _ip) { m_websocketIP = _ip; }

	//! @brief Will return the IP address of the websocket
	std::string websocketIP(void) const { return m_websocketIP; }

	virtual std::string toJSON(void) const override;

	//! @brief Will return the port numbers of this service
	virtual std::list<unsigned long long> portNumbers(void) const override;

private:

	Service *					m_connectedService;
	std::string					m_websocketIP;

	RelayService() = delete;
	RelayService(const RelayService &) = delete;

};
