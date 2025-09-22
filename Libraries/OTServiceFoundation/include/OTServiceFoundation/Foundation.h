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

		//! @brief Perform action wrapper function.
		//! Call this method directly from the perform action endpoint.
		//! @param _json The JSON type string containing the action and required parameters.
		//! @param _senderURL The URL of the sender of the action.
		//! @return Copy of the response string. The memory of this value will be deallocated by the framework.
		OT_SERVICEFOUNDATION_API_EXPORTONLY const char* performAction(const std::string& _json, const std::string& _senderURL);

		//! @brief Perform action wrapper function for one way TLS messages.
		//! Call this method directly from the perform one way TLS action endpoint.
		//! @param _json The JSON type string containing the action and required parameters.
		//! @param _senderURL The URL of the sender of the action.
		//! @return Copy of the response string. The memory of this value will be deallocated by the framework.
		OT_SERVICEFOUNDATION_API_EXPORTONLY const char* performActionOneWayTLS(const std::string& _json, const std::string& _senderURL);

		//! @brief Queue action wrapper function.
		//! Call this method directly from the queue action endpoint.
		//! @param _json The JSON type string containing the action and required parameters.
		//! @param _senderURL The URL of the sender of the action.
		//! @return Copy of the response string. The memory of this value will be deallocated by the framework.
		OT_SERVICEFOUNDATION_API_EXPORTONLY const char* queueAction(const std::string& _json, const std::string& _senderURL);

		//! @brief Get service URL
		//! Call this method directly from the get service URL endpoint.
		//! @return Copy of the service URL string. The memory of this value will be deallocated by the framework.
		OT_SERVICEFOUNDATION_API_EXPORTONLY const char* getServiceURL();

		//! @brief Initialize the foundation component.
		//! Call this method directly from the service initialization endpoint.
		//!
		//! In Debug mode ot::ExternalServicesComponent::startup will be called.
		//! Afterwards ot::ExternalServicesComponent::init will be called.
		//! 
		//! In Release mode only ot::ExternalServicesComponent::startup will be called.
		//! @param _localDirectoryServiceURL The URL of the local directory service.
		//! @param _ownURL The URL of this service.
		//! @param _localSessionServiceURL The URL of the local session service.
		//! @param _sessionID The session ID this service is running in.
		//! @param _application The application object that is using this component.
		//! @param _explicitDebug If true, the debug initialization will be performed even if running in release mode.
		OT_SERVICEFOUNDATION_API_EXPORTONLY int init(const std::string& _localDirectoryServiceURL, const std::string& _ownURL, const std::string& _localSessionServiceURL, const std::string& _sessionID, ApplicationBase* _application, bool _explicitDebug);

	} // namespace foundation

}