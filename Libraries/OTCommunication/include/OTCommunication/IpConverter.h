// @otlicense

#pragma once

// OpenTwin header
#include "OTCommunication/CommunicationAPIExport.h"

 // std header
#include <string>

namespace ot {

	class OT_COMMUNICATION_API_EXPORT IpConverter {
	public:
		IpConverter() {}
		virtual ~IpConverter() {}

		//! @brief Will extract the IP address of the sender and replace the port with the one provided
		//! @param _senderIP The string containing the IP address
		//! @param _senderPort The port of the sender
		static std::string filterIpFromSender(const std::string& _senderIP, const std::string& _senderPort);

		//! @brief Will return the port from the provided IP
		//! @param _ip The IP adress
		static std::string portFromIp(const std::string& _ip);

		//! @brief Will return the host from the provided IP
		//! @param _ip The IP adress
		static std::string hostFromIp(const std::string& _ip);

	private:
		IpConverter(const IpConverter&) = delete;
	};

}
