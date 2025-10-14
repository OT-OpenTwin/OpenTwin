//! @file ApplicationBase.h
//! @author Alexander Kuester (alexk95)
//! @date February 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin System header
#include "OTSystem/Exception.h"                      // Exceptions

// OpenTwin Core header
#include "OTCore/JSON.h"                             // JSON related methods and types
#include "OTCore/CoreTypes.h"                        // Core types
#include "OTCore/ServiceBase.h"                      // Base class for services
#include "OTCore/ReturnMessage.h"                    // Return message related methods and types
#include "OTCore/LogDispatcher.h"                    // Logging related methods and types

// OpenTwin Gui header
#include "OTGui/PropertyGridCfg.h"                   // Property grid configuration

// OpenTwin Communication header
#include "OTCommunication/Msg.h"                     // Message related methods and types
#include "OTCommunication/ActionHandler.h"           // Action handle connection

// OpenTwin ServiceFoundation header
#include "OTServiceFoundation/FoundationAPIExport.h" // OT_SERVICEFOUNDATION_API_EXPORT

// OpenTwin ModelEntities header
#include "EntityInformation.h"                        // Entity information

// std header
#include <map>
#include <list>
#include <string>

#pragma warning(disable : 4251)

namespace ot {

	namespace components {
		class UiComponent;
		class ModelComponent;
	}
	
	namespace intern {
		class ExternalServicesComponent;
	}

	class ModalCommandBase;
	class AbstractUiNotifier;
	class AbstractSettingsItem;
	class AbstractModelNotifier;

	//! @brief Base class for applications.
	//! The ApplicationBase class is the base class for all applications that are built on top of the OpenTwin framework.
	//! It provides a set of virtual methods that can be overloaded to implement application specific functionality.
	class OT_SERVICEFOUNDATION_API_EXPORT ApplicationBase : public ServiceBase {
		OT_DECL_NOCOPY(ApplicationBase)
		OT_DECL_NOMOVE(ApplicationBase)
		OT_DECL_NODEFAULT(ApplicationBase)
	public:
		//! @brief Constructor
		//! @param _serviceName The name of the service.
		//! @param _serviceType The type of the service.
		//! @param _uiNotifier An optional notifier that will be used to notify the UI about changes in the application.
		//! @param _modelNotifier An optional notifier that will be used to notify the model service about changes in the application.
		ApplicationBase(const std::string & _serviceName, const std::string & _serviceType, AbstractUiNotifier * _uiNotifier = nullptr, AbstractModelNotifier * _modelNotifier = nullptr);
		virtual ~ApplicationBase();

		// ####################################################################################################################################################################################################################################################################################

		// Setter

		//! @brief Will set the database URL of the database this application is using.
		//! @param _url The URL of the database to set.
		void setDataBaseURL(const std::string & _url) { m_databaseURL = _url; }

		//! @brief Will set the site ID this application is running on.
		//! @param _id The site ID to set.
		void setSiteID(const std::string & _id) { m_siteID = _id; }

		//! @brief Will set the session service URL.
		//! @param _url The session service URL.
		void setSessionServiceURL(const std::string & _url);

		//! @brief Set the project type.
		void setProjectType(const std::string& _type) { m_projectType = _type; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		//! @brief Will return the path of the deployment folder.
		//! Returns an empty string in case of an error.
		std::string deploymentPath() const;

		//! @brief Will return the URL of the authorization service.
		const std::string& getAuthorizationUrl() const { return m_authUrl; };

		//! @brief Will return the URL of the database.
		const std::string& getDataBaseURL() const { return m_databaseURL; }

		//! @brief Will return the name of the user collection in the database.
		const std::string& getDbUserCollection() const { return m_dbUserCollection; };

		//! @brief Will return the site ID this application is running on.
		const std::string& getSiteID() const { return m_siteID; }

		//! @brief Will return the session service URL.
		std::string getSessionServiceURL() const { return (m_sessionService ? m_sessionService->getServiceURL() : ""); };

		const std::string& getLMSUrl() const { return m_lmsUrl; };

		//! @brief Will return the session ID this service is running in.
		const std::string& getSessionID() const { return m_sessionID; }
		
		//! @brief The current project type.
		const std::string& getProjectType() const { return m_projectType; };

		//! @brief The current project name.
		const std::string& getProjectName() const { return m_projectName; };

		//! @brief The current collection name.
		const std::string& getCollectionName() const { return m_collectionName; };

		//! @brief Will return true if a UI is running in the session
		bool isUiConnected() const { return m_uiComponent != nullptr; }

		//! @brief Will return true if a model service is running in the session
		bool isModelConnected() const { return m_modelComponent != nullptr; }

		//! @brief Will return the model component used in this application
		//! The model component only exists when a model service is running in the session
		components::ModelComponent * getModelComponent() const { return m_modelComponent; }

		//! @brief Will return the ui component used in this application
		//! The UI component only exists when a ui service is running in the session
		components::UiComponent * getUiComponent() const { return m_uiComponent; }

		//! @brief Will return the service with the specified ID
		//! @param _id The ID of the requested service
		ServiceBase * getConnectedServiceByID(serviceID_t _id);

		//! @brief Will return the service with the specified name
		//! @param _name The name of the requested service
		ServiceBase * getConnectedServiceByName(const std::string & _name);

		//! @brief Will return the uiNotifier attached to this application
		AbstractUiNotifier * getUiNotifier() { return m_uiNotifier; }

		//! @brief Will return the modelNotifier attached to this application
		AbstractModelNotifier * getModelNotifier() { return m_modelNotifier; }

		const std::list<UID>& getSelectedEntities() const { return m_selectedEntities; };
		const std::list<ot::EntityInformation>& getSelectedEntityInfos() const { return m_selectedEntityInfos; };

		void prefetchDocumentsFromStorage(const std::list<UID>& _entities);
		void prefetchDocumentsFromStorage(const std::list<ot::EntityInformation>& _entityInfo);
		UID getPrefetchedEntityVersion(UID _entityID);
		const std::map<UID, UID>& getPrefetchedEntityVersions() const { return m_prefetchedEntityVersions; };

		const std::string& getLogInUserName() const { return m_loggedInUserName; };
		const std::string& getLogInUserPassword() const { return m_loggedInUserPassword; };

		const std::string& getDataBaseUserName() const { return m_dataBaseUserName; };
		const std::string& getDataBaseUserPassword() const { return m_dataBaseUserPassword; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Modal Commands

		void addModalCommand(ModalCommandBase *command);
		void removeModalCommand(ModalCommandBase *command);
		std::string processActionWithModalCommands(const std::string & _action, JsonDocument& _doc);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Messaging

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
		bool sendMessage(bool _queue, const std::string & _serviceName, const JsonDocument& _doc, std::string& _response, const ot::msg::RequestFlags& _requestFlags = ot::msg::DefaultFlags);

		//! @brief Will send the message to the service with the specified name
		//! @param _queue If true, the message will be queued
		//! @param _serviceName The name of the service
		//! @param _doc The document containing the message
		//! @param _prefetchIds A list containing the prefetched IDs
		//! @param _response The reponse will be written here
		bool sendMessage(bool _queue, const std::string & _serviceName, const JsonDocument& _doc, std::list<std::pair<UID, UID>> & _prefetchIds, std::string& _response, const ot::msg::RequestFlags& _requestFlags = ot::msg::DefaultFlags);

		//! @brief Will be called whenever the selection in the UI has changed.
		virtual void modelSelectionChanged() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Proptected: Virtual functions to be overloaded by the application

	protected:

		//! @brief Initialize the service.
		//! Will be called when the service was successfully started
		//! and the hppt and/or websocket connection is established.
		//! The services in the session might not be connected at this point.
		virtual void initialize() {};

		//! @brief Run the service.
		//! Will be called when the service was successfully started, 
		//! the hppt and/or websocket connection is established,
		//! all services in the session have connected and the service may start its work.
		virtual void run() {};

		//! @brief Will be called whenever a action should be processed. Core actions will be processed in the base and will not be forwarded to this function (see documentation).
		//! @param _action The action that should be processed.
		//! @param _doc The document containing all the information.
		virtual std::string processAction(const std::string& _action, JsonDocument& _doc) {
			OT_LOG_W("Unsupported action \"" + _action + "\" received.");
			return ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, "Unsupported action \"" + _action + "\" received");
		};

		//! @brief Will be called when a UI connected to the session and is ready to work.
		virtual void uiConnected(components::UiComponent* _ui) {};

		//! @brief Will be called when a UI disconnected from the session (is already closed).
		virtual void uiDisconnected(const components::UiComponent* _serviceInfo) {};

		//! @brief Will be called when a model service connected to the session and is ready to work.
		virtual void modelConnected(components::ModelComponent* _model) {};

		//! @brief Will be called when a mode service disconnected from the session (is already closed).
		virtual void modelDisconnected(const components::ModelComponent* _serviceInfo) {};

		//! @brief Will be called when a service is connected to the session (UI and model services will not trigger this function).
		//! @param _service The service information.
		virtual void serviceConnected(const ServiceBase& _service) {};

		//! @brief Will be called when a service is disconnected from the session (is already closed) (UI and model services will not trigger this function).
		//! @param _service The service information.
		virtual void serviceDisconnected(const ServiceBase& _service) {};

		//! @brief Will be called before the whole session starts to shut down (shut down can not be stopped)
		//! At this point all services, that are listed as connected, are still reachable.
		virtual void preShutdown() {};

		//! @brief Will be called when the application is shutting down (shut down can not be stopped)
		//! At this point all services, that are listed as connected, might not be reachable anymore.
		//! This function will be called after the preShutDown call.
		virtual void shuttingDown() {};

		//! @brief Create settings that your application uses that are editable in the uiService.
		//! The created class will be deleted after used for sending or synchronizing with the database.
		//! The created settings will be requested upon Service startup to synchronize with the database,
		//! aswell as when the uiService is connected.
		virtual PropertyGridCfg createSettings() const { return PropertyGridCfg(); };

		//! @brief This function will be called when the settings were synchronized with the database.
		//! At this point the values from the dataset should be stored since the dataset will be deleted after this function call.
		//! @param The dataset that contains all values.
		virtual void settingsSynchronized(const PropertyGridCfg& _config) {};

		//! @brief This function will be called when the settings were changed in the uiService.
		//! The value of the provided item should be stored.
		//! If the change of the item will change the item visibility of any settings item, this function should return true, otherwise false. 
		//! When returning true, the function createSettings() will be called and the created dataset will be send to the uiService to update the Settings in the dialog.
		//! @param The item that has been changed in the uiService (instance will be deleted after this function call).
		virtual bool settingChanged(const Property* _property) { return false; };

		//! @brief Will be called whenever a property in the property grid has been changed.
		virtual void propertyChanged(ot::JsonDocument& _doc) {};

		//! @brief Will be called whenever the log flags have been changed.
		//! @param _flags Newly set flags.
		virtual void logFlagsChanged(const ot::LogFlags& _flags) {};

		//! @brief Initialize the default template.
		//! Will be called after the connection to the database was established.
		virtual void initializeDefaultTemplate();

		bool storeSettingToDataBase(const PropertyGridCfg& _config, const std::string& _databaseURL, const std::string& _siteID, const std::string& _userName, const std::string& _userPassword, const std::string& _userCollection);

		PropertyGridCfg getSettingsFromDataBase(const std::string& _databaseURL, const std::string& _siteID, const std::string& _userName, const std::string& _userPassword, const std::string& _userCollection);

		// ##########################################################################################################################################

		// Private: Action handler

	private:
		friend class intern::ExternalServicesComponent;
		ActionHandler m_actionHandler;
		std::string handleKeySequenceActivated(JsonDocument& _document);
		ot::ReturnMessage handleSettingsItemChanged(JsonDocument& _document);
		ot::ReturnMessage handleRegisterNewLMS(JsonDocument& _document);

		// ##########################################################################################################################################

		// Private: Internal functions

		//! @brief Will set the session ID of the application
		//! @param _id The session ID to set
		void setSessionIDPrivate(const std::string& _id);

		bool initializeDataBaseConnectionPrivate();

		void serviceConnectedPrivate(const ot::ServiceBase& _service);
		void serviceDisconnectedPrivate(serviceID_t _id);
		void shuttingDownPrivate(bool _requestedAsCommand);

		bool                                m_uiMessageQueuingEnabled;


		std::map<std::string, ServiceBase*>	m_serviceNameMap;
		std::map<serviceID_t, ServiceBase*>	m_serviceIdMap;
		
		ServiceBase*                        m_sessionService;
		ServiceBase*                        m_directoryService;
		components::ModelComponent*         m_modelComponent;
		components::UiComponent*            m_uiComponent;
		AbstractUiNotifier*                 m_uiNotifier;
		AbstractModelNotifier*              m_modelNotifier;

		std::string                         m_authUrl;
		std::string                         m_databaseURL;
		std::string                         m_siteID;
		std::string                         m_lmsUrl;

		std::string                         m_sessionID;
		std::string                         m_projectName;
		std::string                         m_collectionName;
		std::string                         m_projectType;

		std::string                         m_loggedInUserName;
		std::string                         m_loggedInUserPassword;
		std::string                         m_dataBaseUserName;
		std::string                         m_dataBaseUserPassword;

		std::list<ot::ModalCommandBase*>    m_modalCommands;

		std::list<UID>                      m_selectedEntities;
		std::list<ot::EntityInformation>    m_selectedEntityInfos;
		std::map<UID, UID>                  m_prefetchedEntityVersions;

		std::string                         m_dbUserCollection;
	};

}