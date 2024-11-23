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
#include "OTServiceFoundation/EntityInformation.h"
#include "EntityGeometry.h"

#include "InfoFileManager.h"

// C++ header
#include <string>
#include <list>
#include <map>

// Forward declaration
class EntityBase;
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class EntityUnits;

class Application : public ot::ApplicationBase {
public:
	Application();
	virtual ~Application();

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
	virtual ot::PropertyGridCfg createSettings(void) const override { return ot::PropertyGridCfg(); };

	//! @brief This function will be called when the settings were synchronized with the database
	//! At this point the values from the dataset should be stored since the dataset will be deleted after this function call
	//! @param The dataset that contains all values
	virtual void settingsSynchronized(const ot::PropertyGridCfg& _dataset) override {};

	//! @brief This function will be called when the settings were changed in the uiService
	//! The value of the provided item should be stored.
	//! If the change of the item will change the item visibility of any settings item, this function should return true,
	//! otherwise false. When returning true, the function createSettings() will be called and the created dataset will be
	//! send to the uiService to update the Settings in the dialog
	//! @param The item that has been changed in the uiService (instance will be deleted after this function call)
	virtual bool settingChanged(const ot::Property* _item) override { return false; };
	
	// ##################################################################################################################################

	OT_HANDLER(handleExecuteModelAction, Application, OT_ACTION_CMD_MODEL_ExecuteAction, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleModelSelectionChanged, Application, OT_ACTION_CMD_MODEL_SelectionChanged, ot::SECURE_MESSAGE_TYPES);

	void modelSelectionChangedNotification(void);

	void importProject(void);
	void setCSTFile(void);
	void showInformation(void);
	void commitChanges(void);
	void getChanges(void);
	void setLocalFileName(const std::string& hostName, const std::string& fileName);

	void EnsureVisualizationModelIDKnown(void);

	void changeUnits(const std::string &content);
	void changeMaterials(const std::string &content);
	void shapeInformation(const std::string &content);
	void processSingleUnit(const std::string& unitName, std::stringstream& buffer, EntityUnits* units, bool& changed);
	bool processSingleMaterial(std::stringstream& buffer, std::map<std::string, bool> &materialProcessed);
	void readDoubleTriple(const std::string& line, double& a, double& b, double& c);
	void shapeTriangles(std::list<std::string>& shapeNames, std::list<std::string>& shapeTriangles, std::list<std::string>& shapeHash);
	void result1D(bool appendData, std::string &data, size_t uncompressedDataLength);
	void storeShape(const std::string& name, const std::string& triangles, const std::string& materialsFolder, ot::UID materialsFolderID);
	void createFacets(const std::string& data, std::vector<Geometry::Node>& nodes, std::list<Geometry::Triangle>& triangles, std::list<Geometry::Edge>& edges);
	void writeProjectInformation(const std::string &simpleFileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames);
	bool readProjectInformation(std::string &simpleFileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames);
	bool isProjectInitialized();
	std::string getLocalFileName(const std::string& hostName);
	void addHostNameAndFileName(const std::string& hostName, const std::string& fileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames);
	std::string getSimpleFileName();
	long long getCurrentModelEntityVersion(void);

private:
	void uploadNeeded(ot::JsonDocument& _doc);
	void downloadNeeded(ot::JsonDocument& _doc);
	void filesUploaded(ot::JsonDocument& _doc);
	
	std::list<ot::UID>		selectedEntities;
	ot::UID					visualizationModelID;

	std::map<std::string, std::tuple<double, double, double>> materialColors;
	std::map<std::string, std::string> shapeMaterials;
	InfoFileManager infoFileManager;
};
