#pragma once

#include "OTCore/Logger.h"
#include "OTCore/Flags.h"
#include "OTCommunication/CommunicationAPIExport.h"
#include "OTCommunication/CommunicationTypes.h"

#include <string>
#include <ostream>
#include <mutex>
#include <list>

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
		std::string				m_loggingServiceURL;

		ServiceLogNotifier();
		virtual ~ServiceLogNotifier();

		ServiceLogNotifier(ServiceLogNotifier&) = delete;
		ServiceLogNotifier& operator = (ServiceLogNotifier&) = delete;
	};

}