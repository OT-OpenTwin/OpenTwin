// @otlicense

//! @file ExternalServicesComponent.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date February 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/SystemInformation.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTCommunication/ActionHandler.h"
#include "OTCommunication/ServiceRunData.h"
#include "OTCommunication/ServiceInitData.h"

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
			//! @param _application The application object that is using this component. The caller keeps ownership of this instance.
			//! @param _ownURL The URL of this service.
			//! @return 0 if successful, otherwise an error code.
			int startup(ApplicationBase* _application, const std::string& _ownURL);

			//! @brief Will initialize the service.
			ot::ReturnMessage init(const ot::ServiceInitData& _initData);

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

		private:
			// #####################################################################################################################################

			// Private functions

			ActionHandler m_actionHandler;

			void handleSetLogFlags(JsonDocument& _document);
			std::string handleInitialize(JsonDocument& _document);
			void handleServiceConnected(JsonDocument& _document);
			void handleServiceDisconnected(JsonDocument& _document);
			void handleShutdownRequestByService();
			void handleServiceShutdown();
			void handleRun(JsonDocument& _document);
			void handlePreShutdown();
			void handleEmergencyShutdown();

			// #####################################################################################################################################

			// Private member
			std::mutex m_actionDispatching;

			ApplicationBase * m_application;			// The application object
			ComponentState    m_componentState;
			SystemInformation m_systemLoad;
			
			ExternalServicesComponent();
			virtual ~ExternalServicesComponent();
		}; // Class external services component

	} // namespace intern

} // namespace ot