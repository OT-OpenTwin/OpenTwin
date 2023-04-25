#pragma once
#pragma warning(disable : 4251)

#include "OpenTwinCore/rJSON.h"
#include "OpenTwinFoundation/FoundationAPIExport.h"

#include <string>

namespace ot {

	class ApplicationBase;

	namespace components {

		class UiComponent;

		class OT_SERVICEFOUNDATION_API_EXPORT UiPluginComponent {
		public:
			UiPluginComponent(unsigned long long _pluginUID, const std::string& _pluginName, ApplicationBase * _application, UiComponent * _uiComponent);
			virtual ~UiPluginComponent();

			// ##################################################################################################
			
			//! @brief Will send a queued message to the plugin
			//! @param _action The action identicator
			//! @param _message The action message containing additional information
			void sendQueuedMessage(const std::string& _action, const std::string& _message);

			//! @brief Will send a queued message to the plugin
			//! @param _action The action identicator
			//! @param _message The action message as a json object
			void sendQueuedMessage(const std::string& _action, OT_rJSON_doc& _object);

			// ##################################################################################################

			//! @brief The plugin UID
			unsigned long long pluginUID(void) const { return m_pluginUID; }

			//! @brief The plugin Name
			const std::string& pluginName(void) const { return m_pluginName; }

		private:
			ApplicationBase *		m_application;
			UiComponent *			m_uiComponent;
			unsigned long long		m_pluginUID;
			std::string				m_pluginName;

			UiPluginComponent(UiPluginComponent&) = delete;
			UiPluginComponent& operator = (UiPluginComponent&) = delete;
		};

	}

}