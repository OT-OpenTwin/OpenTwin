// @otlicense
// File: IpConverter.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
