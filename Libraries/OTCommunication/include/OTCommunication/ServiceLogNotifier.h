// @otlicense
// File: ServiceLogNotifier.h
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
#include "OTCore/AbstractLogNotifier.h"
#include "OTSystem/Flags.h"
#include "OTCommunication/CommunicationAPIExport.h"
#include "OTCommunication/CommunicationTypes.h"

// std header
#include <mutex>
#include <string>

#pragma warning (disable: 4251)

namespace ot {

	class OT_COMMUNICATION_API_EXPORT ServiceLogNotifier : public AbstractLogNotifier {
	public:
		static ServiceLogNotifier& instance(void);
		static ServiceLogNotifier& initialize(const std::string& _serviceName, const std::string& _loggingServiceUrl, bool _addCoutReceiver = false);
	
		// ###################################################################################################################################

		// Setter/Getter

		void setLoggingServiceURL(const std::string& _url);
		const std::string& loggingServiceURL(void) const { return m_loggingServiceURL; };

		// ###################################################################################################################################

		void log(const LogMessage& _message);

	private:
		std::mutex              m_mutex;
		std::string				m_loggingServiceURL;

		ServiceLogNotifier();
		virtual ~ServiceLogNotifier();

		ServiceLogNotifier(ServiceLogNotifier&) = delete;
		ServiceLogNotifier& operator = (ServiceLogNotifier&) = delete;
	};

}