/*
 * Application.h
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

#pragma once

// Open twin header
#include "OTServiceFoundation/ApplicationBase.h"		// Base class
#include "OTServiceFoundation/MenuButtonDescription.h"

// C++ header
#include <string>

#include "TableHandler.h"
#include "DataCategorizationHandler.h"
#include "TabledataToResultdataHandler.h"
#include "TouchstoneToResultdata.h"
#include "ResultCollectionMetadataAccess.h"
#include "RangeSelectionVisualisationHandler.h"
#include "BatchedCategorisationHandler.h"


#include "OTCore/FolderNames.h"
#include "OTServiceFoundation/UILockWrapper.h"

// Forward declaration
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class Application : public ot::ApplicationBase {
public:
	static Application * instance(void);
	static void deleteInstance(void);

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called when the service was successfully started, the hppt and/or websocket connection is established and the service may start its work
	virtual void run(void) override;

	//! @brief Will be called whenever a action should be processed. Core actions will be processed in the base and will not be forwarded to this function (see documentation)
	//! @param _action The action that should be processed
	//! @param _doc The document containing all the information
	virtual std::string processAction(const std::string & _action, ot::JsonDocument& _doc) override;

	//! @brief Will be called whenever a message should be processed. Core messages will be processed in the base and will not be forwarded to this function (see documentation)
	//! @param _sender The service that this message was sent from
	//! @param _message The message that should be processed
	//! @param _doc The document containing all the information
	virtual std::string processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc) override;

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	//! @brief Will be called when a UI disconnected from the session (is already closed)
	virtual void uiDisconnected(const ot::components::UiComponent * _ui) override;

	//! @brief Will be called when a model service connected to the session and is ready to work
	virtual void modelConnected(ot::components::ModelComponent * _model) override;

	//! @brief Will be called when a mode service disconnected from the session (is already closed)
	virtual void modelDisconnected(const ot::components::ModelComponent * _model) override;

	//! @brief Will be called when a service is connected to the session (UI and model services will not trigger this function)
	//! @param _service The service information
	virtual void serviceConnected(ot::ServiceBase * _service) override;

	//! @brief Will be called when a service is disconnected from the session (is already closed) (UI and model services will not trigger this function)
	//! @param _service The service information
	virtual void serviceDisconnected(const ot::ServiceBase * _service) override;

	//! @brief Will be called before the whole session starts to shut down (shut down can not be stopped)
	//! At this point all services, that are listed as connected, are still reachable.
	virtual void preShutdown(void) override;

	//! @brief Will be called when the application is shutting down (shut down can not be stopped)
	virtual void shuttingDown(void) override;

	//! @brief Will return true if this application requires a relay service for a websocket connection
	virtual bool startAsRelayService(void) const override;
	
	//! @brief Create settings that your application uses that are editable in the uiService
	//! The created class will be deleted after used for sending or synchronizing with the database.
	//! The created settings will be requested upon Service startup to synchronize with the database,
	//! aswell as when the uiService is connected
	virtual ot::PropertyGridCfg createSettings(void) const override;

	//! @brief This function will be called when the settings were synchronized with the database
	//! At this point the values from the dataset should be stored since the dataset will be deleted after this function call
	//! @param The dataset that contains all values
	virtual void settingsSynchronized(const ot::PropertyGridCfg& _dataset) override;

	//! @brief This function will be called when the settings were changed in the uiService
	//! The value of the provided item should be stored.
	//! If the change of the item will change the item visibility of any settings item, this function should return true,
	//! otherwise false. When returning true, the function createSettings() will be called and the created dataset will be
	//! send to the uiService to update the Settings in the dialog
	//! @param The item that has been changed in the uiService (instance will be deleted after this function call)
	virtual bool settingChanged(const ot::Property* _item) override;
	
	// ##################################################################################################################################

	// Add your custom functions/ members here

	//! A handler can be created to handle the the specified action
	//! In this example the first parameter is the name of the callback function
	//! The second parameter is the class name where the handler is created at
	//! The last parameter is a C or C++ String containing the action name
	//OT_HANDLER(myHandleFunctionName, Application, "actionToHandle");

	virtual void modelSelectionChanged(void) override;
	virtual ~Application();

	void propertyChanged(ot::JsonDocument& _doc) override;
private:
	Application();

	std::mutex m_onlyOneActionPerTime;
	UILockWrapper* m_twoPartsAction = nullptr;

	ot::MenuButtonDescription m_buttonImportTouchstone;
	
	ot::MenuButtonDescription m_buttonCreateMSMDEntry;
	ot::MenuButtonDescription m_buttonCreateRMDEntry;
	ot::MenuButtonDescription m_buttonCreateParameterEntry;
	ot::MenuButtonDescription m_buttonCreateQuantityEntry;
	ot::MenuButtonDescription m_buttonAutomaticCreationMSMD;
	ot::MenuButtonDescription m_buttonAddBatchCreator;

	ot::MenuButtonDescription m_buttonCreateDataCollection;

	const std::string _dataSourcesFolder = "Files";
	const std::string _scriptsFolder = ot::FolderNames::PythonScriptFolder;
	const std::string _tableFolder = "Tables";
	
	const std::string _previewTableNAme = "Preview";
	const std::string _datasetFolder = ot::FolderNames::DatasetFolder;

	ot::UID _visualizationModel = -1;
	   
	TableHandler* _tableHandler = nullptr;
	DataCategorizationHandler* m_parametrizedDataHandler = nullptr;
	TabledataToResultdataHandler* _tabledataToResultdataHandler = nullptr;
	TouchstoneToResultdata* _touchstoneToResultdata = nullptr;
	
	RangeSelectionVisualisationHandler m_rangleSelectionVisualisationHandler;
	BatchedCategorisationHandler m_batchedCategorisationHandler;
	void HandleSelectionChanged();
	void ProcessActionDetached(const std::string& _action, ot::JsonDocument _doc);
};
