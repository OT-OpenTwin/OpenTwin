/*
 *  ExternalServicesComponent.h
 *
 *  Created on: 01/02/2021
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2021, OpenTwin
 */

#pragma once

// OpenTwin header
#include "OTServiceFoundation/OTObject.h"
#include "OTCommunication/actionTypes.h"

// std header
#include <string>		// string
#include <mutex>		// mutex

namespace ot {

	class ApplicationBase;

	namespace intern {

		//! This is the main class interacting with external services.
		//! Use the static instance() function to get the global component that should be used in this service
		class ExternalServicesComponent : public OTObject {
		public:
			enum ComponentState {
				WaitForStartup,
				WaitForInit,
				Ready
			};

			//! @brief Will return the global instance of this component since it should exist only once in the service.
			//todo: replace with reference instead of pointer
			static ExternalServicesComponent * instance();

			static bool hasInstance(void);

			//! @brief Will delete the instance of this component
			static void deleteInstance();

			// ##########################################################################################################################################

			// Mandatory functions

			int startup(ApplicationBase * _application, const std::string& _localDirectoryServiceURL, const std::string& _ownURL);

			//! @brief Will initialize the service
			//! @param _siteID The site ID this service is running on
			//! @param _ownIP The IP address of this service
			//! @param _sessionServiceIP The IP address of the session service this service was started from/ should interact with
			//! @param _sessionID The ID of the session this service is running at
			std::string init(
				const std::string &					_sessionServiceURL,
				const std::string &					_sessionID
			);			
			std::string initDebugExplicit(
				const std::string &					_sessionServiceURL,
				const std::string &					_sessionID
			);

			//! @brief Will perform the provided action
			//! @param _json The JSON type string containing the action and required parameter
			//! @param _sender The sender IP of the service sending the action
			//! @return The response to the request or the error / warning text
			std::string dispatchAction(
				const std::string &					_json,
				const std::string &					_sender,
				ot::MessageType						_messageType
			);

			// #####################################################################################################################################

			//! @brief Will shutdown the service
			//! @param _receivedAsCommand If true, the shutdown request was send from the session service
			void shutdown(bool _requestedAsCommand);

			//! @brief Will return the application
			ApplicationBase * application(void) const { return m_application; };

			ComponentState componentState(void) const { return m_componentState; };

		private:
			// #####################################################################################################################################

			// Private functions

			OT_HANDLER(handleInitialize, ExternalServicesComponent, OT_ACTION_CMD_Init, ot::SECURE_MESSAGE_TYPES);
			OT_HANDLER(handleServiceConnected, ExternalServicesComponent, OT_ACTION_CMD_ServiceConnected, ot::SECURE_MESSAGE_TYPES);
			OT_HANDLER(handleServiceDisconnected, ExternalServicesComponent, OT_ACTION_CMD_ServiceDisconnected, ot::SECURE_MESSAGE_TYPES);
			OT_HANDLER(handleShutdownRequestByService, ExternalServicesComponent, OT_ACTION_CMD_ShutdownRequestedByService, ot::SECURE_MESSAGE_TYPES);
			OT_HANDLER(handleMessage, ExternalServicesComponent, OT_ACTION_CMD_Message, ot::SECURE_MESSAGE_TYPES);
			OT_HANDLER(handleServiceShutdown, ExternalServicesComponent, OT_ACTION_CMD_ServiceShutdown, ot::SECURE_MESSAGE_TYPES);
			OT_HANDLER(handleRun, ExternalServicesComponent, OT_ACTION_CMD_Run, ot::SECURE_MESSAGE_TYPES);
			OT_HANDLER(handlePreShutdown, ExternalServicesComponent, OT_ACTION_CMD_ServicePreShutdown, ot::SECURE_MESSAGE_TYPES);
			OT_HANDLER(handleEmergencyShutdown, ExternalServicesComponent, OT_ACTION_CMD_ServiceEmergencyShutdown, ot::SECURE_MESSAGE_TYPES);
			OT_HANDLER(handleUIPluginConnected, ExternalServicesComponent, OT_ACTION_CMD_UI_RequestPluginSuccess, ot::SECURE_MESSAGE_TYPES);

			// #####################################################################################################################################

			// Private member

			ApplicationBase * m_application;			// The application object
			ComponentState    m_componentState;

			ExternalServicesComponent();
			virtual ~ExternalServicesComponent();

			ExternalServicesComponent(ExternalServicesComponent&) = delete;
			ExternalServicesComponent& operator = (ExternalServicesComponent&) = delete;

		}; // Class external services component

	} // namespace intern

	namespace foundation {

		__declspec (dllexport) const char * performAction(
			const std::string &					_json,
			const std::string &					_senderURL
		);

		__declspec (dllexport) const char * performActionOneWayTLS(
			const std::string &					_json,
			const std::string &					_senderURL
		);

		__declspec (dllexport) const char * queueAction(
			const std::string &					_json,
			const std::string &					_senderURL
		);

		__declspec (dllexport) const char * getServiceURL();

		__declspec (dllexport) int init(
			const std::string &					_localDirectoryServiceURL,
			const std::string &					_ownURL,
			const std::string &					_sessionServiceURL,
			const std::string &					_sessionID,
			ApplicationBase *					_application
		);

		__declspec (dllexport) int initDebugExplicit(
			const std::string& _localDirectoryServiceURL,
			const std::string& _ownURL,
			const std::string& _sessionServiceURL,
			const std::string& _sessionID,
			ApplicationBase* _application
		);
	} // namespace foundation

} // namespace ot