//! @file ApplicationBase.h
//!
//! @author Alexander Kuester (alexk95)
//! @date February 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once
#pragma warning(disable : 4251)

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/OTObjectBase.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTServiceFoundation/EntityInformation.h"
#include "OTServiceFoundation/FoundationAPIExport.h"

#include "ClassFactory.h"

// std header
#include <list>
#include <map>
#include <string>

namespace ot {

	namespace components {
		class UiComponent;
		class ModelComponent;
		class UiPluginComponent;
	}
	
	namespace intern {
		class ExternalServicesComponent;
	}

	class AbstractUIPlugin;
	class ModalCommandBase;
	class AbstractUiNotifier;
	class AbstractSettingsItem;
	class AbstractModelNotifier;

	class OT_SERVICEFOUNDATION_API_EXPORT ApplicationBase : public ServiceBase, public OTObjectBase
	{
		OT_DECL_NOCOPY(ApplicationBase)
	public:
		ApplicationBase(const std::string & _serviceName, const std::string & _serviceType, AbstractUiNotifier * _uiNotifier = nullptr, AbstractModelNotifier * _modelNotifier = nullptr);
		virtual ~ApplicationBase();

		// ##########################################################################################################################################

		// Pure virtual functions

		//! @brief Will be called when the service was successfully started, the hppt and/or websocket connection is established and the service may start its work
		virtual void run(void) = 0;

		//! @brief Will be called whenever a action should be processed. Core actions will be processed in the base and will not be forwarded to this function (see documentation)
		//! @param _action The action that should be processed
		//! @param _doc The document containing all the information
		virtual std::string processAction(const std::string & _action, JsonDocument& _doc) = 0;
		
		//! @brief Will be called whenever a message should be processed. Core messages will be processed in the base and will not be forwarded to this function (see documentation)
		//! @param _sender The service that this message was sent from
		//! @param _message The message that should be processed
		//! @param _doc The document containing all the information
		virtual std::string processMessage(ServiceBase * _sender, const std::string & _message, JsonDocument& _doc) = 0;

		//! @brief Will be called when a UI connected to the session and is ready to work
		virtual void uiConnected(components::UiComponent * _ui) = 0;
		
		//! @brief Will be called when a UI plugin was sucessfully connected
		virtual void uiPluginConnected(components::UiPluginComponent * _uiPlugin) = 0;

		//! @brief Will be called when a UI disconnected from the session (is already closed)
		virtual void uiDisconnected(const components::UiComponent * _serviceInfo) = 0;

		//! @brief Will be called when a model service connected to the session and is ready to work
		virtual void modelConnected(components::ModelComponent * _model) = 0;

		//! @brief Will be called when a mode service disconnected from the session (is already closed)
		virtual void modelDisconnected(const components::ModelComponent * _serviceInfo) = 0;

		//! @brief Will be called when a service is connected to the session (UI and model services will not trigger this function)
		//! @param _service The service information
		virtual void serviceConnected(ServiceBase * _service) = 0;

		//! @brief Will be called when a service is disconnected from the session (is already closed) (UI and model services will not trigger this function)
		//! @param _service The service information
		virtual void serviceDisconnected(const ServiceBase * _service) = 0;

		//! @brief Will be called before the whole session starts to shut down (shut down can not be stopped)
		//! At this point all services, that are listed as connected, are still reachable.
		virtual void preShutdown(void) = 0;

		//! @brief Will be called when the application is shutting down (shut down can not be stopped)
		//! At this point all services, that are listed as connected, might not be reachable anymore.
		//! This function will be called after the preShutDown call
		virtual void shuttingDown(void) = 0;

		//! @brief Will return true if this application requires a relay service for a websocket connection
		virtual bool startAsRelayService(void) const = 0;

		//! @brief Create settings that your application uses that are editable in the uiService.
		//! The created class will be deleted after used for sending or synchronizing with the database.
		//! The created settings will be requested upon Service startup to synchronize with the database,
		//! aswell as when the uiService is connected
		virtual PropertyGridCfg createSettings(void) const = 0;

		//! @brief This function will be called when the settings were synchronized with the database.
		//! At this point the values from the dataset should be stored since the dataset will be deleted after this function call
		//! @param The dataset that contains all values
		virtual void settingsSynchronized(const PropertyGridCfg& _config) = 0;

		//! @brief This function will be called when the settings were changed in the uiService.
		//! The value of the provided item should be stored.
		//! If the change of the item will change the item visibility of any settings item, this function should return true, otherwise false. 
		//! When returning true, the function createSettings() will be called and the created dataset will be send to the uiService to update the Settings in the dialog.
		//! @param The item that has been changed in the uiService (instance will be deleted after this function call)
		virtual bool settingChanged(const Property* _property) = 0;

		//! @brief Will return the path of the deployment folder
		//! Returns an empty string in case of an error
		virtual std::string deploymentPath(void) const;

		virtual void modelSelectionChanged(void) {};

		// ####################################################################################################################################################################################################################################################################################

		// Setter

		//! @brief Will set the database URL of the database this application is using
		//! @param _url The URL of the database to set
		void setDataBaseURL(const std::string & _url) { m_databaseURL = _url; }

		//! @brief Will set the site ID this application is running on
		//! @param _id The site ID to set
		void setSiteID(const std::string & _id) { m_siteID = _id; }

		//! @brief Will set the websocket URL that this application is connected to (only required if socket connection is established)
		//! @param _url The websocket URL to set
		void setWebSocketURL(const std::string & _url) { m_websocketURL = _url; }

		//! @brief Will set the local directory service URL
		//! @param _url The directory service URL
		void setDirectoryServiceURL(const std::string& _url);

		//! @brief Will set the session service URL
		//! @param _url The session service URL
		void setSessionServiceURL(const std::string & _url);

		//! @brief Will set the session ID of the application
		//! @param _id The session ID to set
		void setSessionID(const std::string & _id);
		
		//! @brief Set the project type
		void setProjectType(const std::string& _type) { m_projectType = _type; };

		// ##########################################################################################################################################

		// Getter

		//void refreshSessionServiceList(void);

		//! @brief Will return the URL of the database
		std::string dataBaseURL(void) { return m_databaseURL; }

		//! @brief Will return the site ID this application is running on
		std::string siteID(void) { return m_siteID; }

		//! @brief Will return the websocket URL, it is only set if a websocket was required on startup: startAsRelayService() == true
		std::string webSocketURL(void) { return m_websocketURL; }

		//! @brief Will return the session service URL
		std::string sessionServiceURL(void) { return m_sessionService.service->serviceURL(); }

		//! @brief Will return the local directory service URL
		std::string directoryServiceURL(void) { return m_directoryService.service->serviceURL(); }

		//! @brief Will return the session ID this service is running in
		std::string sessionID(void) { return m_sessionID; }
		
		//! @brief The current project type
		const std::string& projectType(void) const { return m_projectType; };

		//! @brief Will return true if a UI is running in the session
		bool isUiConnected(void) const { return m_uiComponent != nullptr; }

		//! @brief Will return true if a model service is running in the session
		bool isModelConnected(void) const { return m_modelComponent != nullptr; }

		//! @brief Will return the model component used in this application
		//! The model component only exists when a model service is running in the session
		components::ModelComponent * modelComponent(void) const { return m_modelComponent; }

		//! @brief Will return the ui component used in this application
		//! The UI component only exists when a ui service is running in the session
		components::UiComponent * uiComponent(void) const { return m_uiComponent; }

		//! @brief Will return the service with the specified ID
		//! @param _id The ID of the requested service
		ServiceBase * getConnectedServiceByID(serviceID_t _id);

		//! @brief Will return the service with the specified name
		//! @param _name The name of the requested service
		ServiceBase * getConnectedServiceByName(const std::string & _name);

		//! @brief Will return the uiNotifier attached to this application
		AbstractUiNotifier * uiNotifier(void) { return m_uiNotifier; }

		//! @brief Will return the modelNotifier attached to this application
		AbstractModelNotifier * modelNotifier(void) { return m_modelNotifier; }

		//! @brief Will return th ui plugin with the specified UID
		components::UiPluginComponent * getUiPluginByUID(unsigned long long _pluginUID);

		//! @brief Will return th ui plugin with the specified Name
		components::UiPluginComponent * getUiPluginByName(const std::string& _pluginName);

		//! @brief Will return true if a ui plugin with the specified UID exists
		bool pluginExists(unsigned long long _pluginUID);

		//! @brief Will return true if a ui plugin with the specified Name exists
		bool pluginExists(const std::string& _pluginName);

		//! @brief Returns a handle to the global class factory for the service
		ClassFactory& getClassFactory(void) { return classFactory; }


		// ##########################################################################################################################################

		// Modal Commands

		void addModalCommand(ModalCommandBase *command);
		void removeModalCommand(ModalCommandBase *command);
		std::string processActionWithModalCommands(const std::string & _action, JsonDocument& _doc);

		// ##########################################################################################################################################

		// IO

		//! @brief Set the message queuing flag
		//! If the flag counter reaches 0 the currently queued message will be flushed
		//! @param _serviceName The name of the service
		//! @param _flag The flag
		void enableMessageQueuing(const std::string & _serviceName, bool _flag);

		//! @brief Will flush the currently queued messages for the service with the provided name
		//! @param _serviceName The name of the service
		void flushQueuedHttpRequests(const std::string & _serviceName);

		//! @brief Will send the message to the service with the specified name
		//! @param _queue If true, the message will be queued
		//! @param _serviceName The name of the service
		//! @param _doc The document containing the message
		//! @param _response The reponse will be written here
		bool sendMessage(bool _queue, const std::string & _serviceName, const JsonDocument& _doc, std::string& _response);

		//! @brief Will send the message to the service with the specified name
		//! @param _queue If true, the message will be queued
		//! @param _serviceName The name of the service
		//! @param _doc The document containing the message
		//! @param _prefetchIds A list containing the prefetched IDs
		//! @param _response The reponse will be written here
		bool sendMessage(bool _queue, const std::string & _serviceName, const JsonDocument& _doc, std::list<std::pair<UID, UID>> & _prefetchIds, std::string& _response);

		//! @brief Will send the message to all other services in this session
		//! @param _queue If true, the message will be queued
		//! @param _message The message to send
		bool broadcastMessage(bool _queue, const std::string& _message);

		//! @brief Will send the message to all other services in this session
		//! @param _queue If true, the message will be queued
		//! @param _doc The JSON Document to send
		bool broadcastMessage(bool _queue, const JsonDocument& _doc);

		void prefetchDocumentsFromStorage(const std::list<UID> &entities);
		void prefetchDocumentsFromStorage(const std::list<ot::EntityInformation> &entityInfo);
		UID getPrefetchedEntityVersion(UID entityID);

		// ##########################################################################################################################################
	protected:

		bool EnsureDataBaseConnection(void);

		bool storeSettingToDataBase(const PropertyGridCfg& _config, const std::string& _databaseURL, const std::string& _siteID, const std::string& _userName, const std::string& _userPassword, const std::string& _userCollection);

		PropertyGridCfg getSettingsFromDataBase(const std::string& _databaseURL, const std::string& _siteID, const std::string& _userName, const std::string& _userPassword, const std::string& _userCollection);

		// ##########################################################################################################################################

		// Member

		bool											m_uiMessageQueuingEnabled;

		struct structServiceInformation {
			ServiceBase *	service;
			int				enabledCounter;
			JsonDocument*	doc;
		};

		std::map<std::string, structServiceInformation>	m_serviceNameMap;

		std::map<serviceID_t, structServiceInformation>	m_serviceIdMap;

		std::map<unsigned long long, components::UiPluginComponent *>	m_uiPluginComponents;
		std::list<std::string>							m_pendingUiPluginRequests;

		structServiceInformation						m_sessionService;
		structServiceInformation						m_directoryService;
		components::ModelComponent *					m_modelComponent;
		components::UiComponent *						m_uiComponent;
		AbstractUiNotifier *							m_uiNotifier;
		AbstractModelNotifier *							m_modelNotifier;

		std::string										m_databaseURL;
		std::string										m_siteID;
		std::string										m_websocketURL;

		std::string										m_sessionID;
		std::string										m_projectName;
		std::string										m_collectionName;
		std::string										m_projectType;

		std::list<ot::ModalCommandBase *>				m_modalCommands;

		std::list<UID>									m_selectedEntities;

		std::map<UID, UID>								m_prefetchedEntityVersions;

		void __serviceConnected(const std::string& _name, const std::string& _type, const std::string& _url, serviceID_t _id);

	private:
		friend class intern::ExternalServicesComponent;

		OT_HANDLER(handleKeySequenceActivated, ApplicationBase, OT_ACTION_CMD_KeySequenceActivated, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleSettingsItemChanged, ApplicationBase, OT_ACTION_CMD_UI_SettingsItemChanged, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleContextMenuItemClicked, ApplicationBase, OT_ACTION_CMD_UI_ContextMenuItemClicked, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleContextMenuItemCheckedChanged, ApplicationBase, OT_ACTION_CMD_UI_ContextMenuItemCheckedChanged, ot::SECURE_MESSAGE_TYPES)
		
		void __serviceDisconnected(const std::string & _name, const std::string & _type, const std::string & _url, serviceID_t _id);
		std::string __processMessage(const std::string & _message, JsonDocument&doc, serviceID_t _senderID);
		void __shuttingDown(bool _requestedAsCommand);
		void __addUiPlugin(components::UiPluginComponent * _component);

		std::string			m_DBuserCollection;
		ClassFactory classFactory;

		ApplicationBase() = delete;
	};

}