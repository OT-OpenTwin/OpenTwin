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