//! @file ExternalServicesComponent.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date February 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyGridCfg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTCommunication/ServiceRunData.h"
#include "OTCommunication/ServiceInitData.h"
#include "OTSystem/SystemInformation.h"

// std header
#include <string> // string
#include <mutex>  // mutex

namespace ot {

	class ApplicationBase;

	namespace intern {
		
		//! This is the main class interacting with external services.
		//! Use the static instance() function to get the global component that should be used in this service
		class ExternalServicesComponent {
			OT_DECL_NOCOPY(ExternalServicesComponent)
			OT_DECL_ACTION_HANDLER(ExternalServicesComponent)
		public:
			enum ComponentState {
				WaitForStartup,
				WaitForInit,
				Ready
			};

			//! @brief Will return the global instance of this component since it should exist only once in the service.
			//todo: replace with reference instead of pointer
			static ExternalServicesComponent& instance(void);

			// ##########################################################################################################################################

			// Mandatory functions

			//! @brief Will initialize the component.
			//! @param _application The application object that is using this component.
			//! @param _ownURL The URL of this service.
			//! @return 0 if successful, otherwise an error code.
			int startup(ApplicationBase* _application, const std::string& _ownURL);

			//! @brief Will initialize the service.
			//! @param _explicitDebug If true, the debug initialization will be performed.
			//! The process ID will be added to the registration request.
			std::string init(const ot::ServiceInitData& _initData, bool _explicitDebug);

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

			void updateSettingsFromDataBase(PropertyGridCfg& _config);

			//! @brief Tell the current cpu and memory load in percent 
			void getCPUAndMemoryLoad(double& globalCPULoad, double& globalMemoryLoad, double& processCPULoad, double& processMemoryLoad);
			std::string handleGetSystemInformation();

			std::string getLoggedInUserName(void) { return m_credentialsUsername; }
			std::string getLoggedInUserPassword(void) { return m_credentialsPassword; }

		private:
			// #####################################################################################################################################

			// Private functions

			OT_HANDLER(handleSetLogFlags, ExternalServicesComponent, OT_ACTION_CMD_SetLogFlags, ot::SECURE_MESSAGE_TYPES)
			OT_HANDLER(handleInitialize, ExternalServicesComponent, OT_ACTION_CMD_Init, ot::SECURE_MESSAGE_TYPES)
			OT_HANDLER(handleServiceConnected, ExternalServicesComponent, OT_ACTION_CMD_ServiceConnected, ot::SECURE_MESSAGE_TYPES)
			OT_HANDLER(handleServiceDisconnected, ExternalServicesComponent, OT_ACTION_CMD_ServiceDisconnected, ot::SECURE_MESSAGE_TYPES)
			OT_HANDLER(handleShutdownRequestByService, ExternalServicesComponent, OT_ACTION_CMD_ShutdownRequestedByService, ot::SECURE_MESSAGE_TYPES)
			OT_HANDLER(handleServiceShutdown, ExternalServicesComponent, OT_ACTION_CMD_ServiceShutdown, ot::SECURE_MESSAGE_TYPES)
			OT_HANDLER(handleRun, ExternalServicesComponent, OT_ACTION_CMD_Run, ot::SECURE_MESSAGE_TYPES)
			OT_HANDLER(handlePreShutdown, ExternalServicesComponent, OT_ACTION_CMD_ServicePreShutdown, ot::SECURE_MESSAGE_TYPES)
			OT_HANDLER(handleEmergencyShutdown, ExternalServicesComponent, OT_ACTION_CMD_ServiceEmergencyShutdown, ot::SECURE_MESSAGE_TYPES)

			// #####################################################################################################################################

			// Private member
			std::mutex m_actionDispatching;

			ApplicationBase * m_application;			// The application object
			ComponentState    m_componentState;
			SystemInformation m_systemLoad;

			std::string m_credentialsUsername;
			std::string m_credentialsPassword;
			
			ExternalServicesComponent();
			virtual ~ExternalServicesComponent();
		}; // Class external services component

	} // namespace intern

} // namespace ot