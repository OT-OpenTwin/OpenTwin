//! @file Foundation.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date February 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTServiceFoundation/FoundationAPIExport.h"

// std header
#include <string>

namespace ot {

	class ApplicationBase;

	namespace foundation {

		OT_SERVICEFOUNDATION_API_EXPORTONLY const char* performAction(
			const std::string& _json,
			const std::string& _senderURL
		);

		OT_SERVICEFOUNDATION_API_EXPORTONLY const char* performActionOneWayTLS(
			const std::string& _json,
			const std::string& _senderURL
		);

		OT_SERVICEFOUNDATION_API_EXPORTONLY const char* queueAction(
			const std::string& _json,
			const std::string& _senderURL
		);

		OT_SERVICEFOUNDATION_API_EXPORTONLY const char* getServiceURL();

		OT_SERVICEFOUNDATION_API_EXPORTONLY int init(
			const std::string& _localDirectoryServiceURL,
			const std::string& _ownURL,
			const std::string& _sessionServiceURL,
			const std::string& _sessionID,
			ApplicationBase* _application
		);

		OT_SERVICEFOUNDATION_API_EXPORTONLY int initDebugExplicit(
			const std::string& _localDirectoryServiceURL,
			const std::string& _ownURL,
			const std::string& _sessionServiceURL,
			const std::string& _sessionID,
			ApplicationBase* _application
		);
	} // namespace foundation

}