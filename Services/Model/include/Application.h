//! @file Application.h
//! 
//! @author Alexander Kuester (alexk95), Peter Thoma
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTServiceFoundation/ApplicationBase.h"
#include "SelectionHandler.h"
#include "FileHandler.h"
#include "ViewVisualisationHandler.h"
#include "CopyPasteHandler.h"

// std header
#include <list>
#include <mutex>

class Model;
class MicroserviceNotifier;

class Application : public ot::ApplicationBase {
	OT_DECL_ACTION_HANDLER(Application)
public:
	enum class ActionType {
		SelectionChanged
	};

	static Application* instance(void);
	
	OT_HANDLER(handleDeleteModel, Application, OT_ACTION_CMD_MODEL_Delete, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleProjectSave, Application, OT_ACTION_CMD_PROJ_Save, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleSelectionChanged, Application, OT_ACTION_CMD_MODEL_SelectionChanged, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleItemRenamed, Application, OT_ACTION_CMD_MODEL_ItemRenamed, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleSetVisualizationModel, Application, OT_ACTION_CMD_SetVisualizationModel, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetVisualizationModel, Application, OT_ACTION_CMD_GetVisualizationModel, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetIsModified, Application, OT_ACTION_CMD_MODEL_GetIsModified, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleSetPropertiesFromJSON, Application, OT_ACTION_CMD_MODEL_SetPropertiesFromJSON, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGenerateEntityIDs, Application, OT_ACTION_CMD_MODEL_GenerateEntityIDs, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleImportTableFile, Application, OT_ACTION_CMD_MODEL_ImportTableFile, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleQueueMessages, Application, OT_ACTION_CMD_MODEL_QueueMessages, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetListOfFolderItems, Application, OT_ACTION_CMD_MODEL_GetListOfFolderItems, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetIDsOfFolderItemsByType, Application, OT_ACTION_CMD_MODEL_GetIDsOfFolderItemsOfType, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleDeleteCurvesFromPlots, Application, OT_ACTION_CMD_MODEL_DeleteCurvesFromPlots, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleUpdateCurvesOfPlot, Application, OT_ACTION_CMD_MODEL_UpdateCurvesOfPlot, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleUpdateVisualizationEntity, Application, OT_ACTION_CMD_MODEL_UpdateVisualizationEntity, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleUpdateGeometryEntity, Application, OT_ACTION_CMD_MODEL_UpdateGeometryEntity, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleModelChangeOperationCompleted, Application, OT_ACTION_CMD_MODEL_ModelChangeOperationCompleted, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleRequestUpdateVisualizationEntity, Application, OT_ACTION_CMD_MODEL_RequestUpdateVisualizationEntity, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleCheckParentUpdates, Application, OT_ACTION_CMD_MODEL_CheckParentUpdates, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleAddEntities, Application, OT_ACTION_CMD_MODEL_AddEntities, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleUpdateTopologyEntity, Application, OT_ACTION_CMD_MODEL_UpdateTopologyEntity, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleAddGeometryOperation, Application, OT_ACTION_CMD_MODEL_AddGeometryOperation, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleDeleteEntity, Application, OT_ACTION_CMD_MODEL_DeleteEntity, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleMeshingCompleted, Application, OT_ACTION_CMD_MODEL_MeshingCompleted, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetEntityInformationByID, Application, OT_ACTION_CMD_MODEL_GetEntityInformationFromID, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetEntityInformationByName, Application, OT_ACTION_CMD_MODEL_GetEntityInformationFromName, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetSelectedEntityInformation, Application, OT_ACTION_CMD_MODEL_GetSelectedEntityInformation, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetEntityChildInformationByName, Application, OT_ACTION_CMD_MODEL_GetEntityChildInformationFromName, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetEntityChildInformationByID, Application, OT_ACTION_CMD_MODEL_GetEntityChildInformationFromID, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetAllGeometryEntitiesForMeshing, Application, OT_ACTION_CMD_MODEL_GetAllGeometryEntitiesForMeshing, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetEntityProperties, Application, OT_ACTION_CMD_MODEL_GetEntityProperties, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetEntityPropertiesByName, Application, OT_ACTION_CMD_MODEL_GetEntityPropertiesFromName, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleUpdatePropertyGrid, Application, OT_ACTION_CMD_MODEL_UpdatePropertyGrid, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetCurrentVisualizationModelID, Application, OT_ACTION_CMD_MODEL_GetCurrentVisModelID, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleAddPropertiesToEntities, Application, OT_ACTION_CMD_MODEL_AddPropertiesToEntities, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleUpdatePropertiesOfEntities, Application, OT_ACTION_CMD_MODEL_UpdatePropertiesOfEntities, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleDeleteProperty, Application, OT_ACTION_CMD_MODEL_DeleteProperty, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleExecuteAction, Application, OT_ACTION_CMD_MODEL_ExecuteAction, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleExecuteFunction, Application, OT_ACTION_CMD_MODEL_ExecuteFunction, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleEntitiesSelected, Application, OT_ACTION_CMD_MODEL_EntitiesSelected, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handlePromptResponse, Application, OT_ACTION_CMD_UI_PromptResponse, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetEntityIdentifier, Application, OT_ACTION_CMD_MODEL_GET_ENTITY_IDENTIFIER, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleGetEntitiesFromAnotherCollection, Application, OT_ACTION_CMD_MODEL_GET_ENTITIES_FROM_ANOTHER_COLLECTION, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleViewsFromProjectType, Application, OT_ACTION_PARAM_MODEL_ViewsForProjectType, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleVisualisationDataRequest, Application, OT_ACTION_CMD_MODEL_RequestVisualisationData, ot::SECURE_MESSAGE_TYPES);
		OT_HANDLER(handleShowTable, Application, OT_ACTION_CMD_UI_TABLE_Setup, ot::SECURE_MESSAGE_TYPES);
		
	// Versions

	OT_HANDLER(handleGetCurrentVersion, Application, OT_ACTION_CMD_MODEL_GetCurrentVersion, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleActivateVersion, Application, OT_ACTION_CMD_MODEL_ActivateVersion, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleVersionSelected, Application, OT_ACTION_CMD_MODEL_VersionSelected, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleVersionDeselected, Application, OT_ACTION_CMD_MODEL_VersionDeselected, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleSetVersionLabel, Application, OT_ACTION_CMD_MODEL_SetVersionLabel, ot::SECURE_MESSAGE_TYPES)

	// ##################################################################################################################################################################################################################

	// Setter / Getter

	Model* getModel(void) const { return m_model; };
	MicroserviceNotifier* getNotifier(void) const { return m_notifier; };

	std::string getEntityInformation(std::list<ot::UID>& _entityIDList) const;

	// ##################################################################################################################################################################################################################

	// Communication

	bool queuedRequestToFrontend(const ot::JsonDocument& _request);
	bool queuedRequestToFrontend(const ot::JsonDocument& _request, std::list<std::pair<ot::UID, ot::UID>>& _prefetchIDs);
	void flushRequestsToFrontEnd();

	// ##################################################################################################################################################################################################################

	// Required functions

	//! @brief Will be called when the service was successfully started, the hppt and/or websocket connection is established and the service may start its work.
	virtual void run(void) override;

	//! @brief Will be called whenever a action should be processed. Core actions will be processed in the base and will not be forwarded to this function (see documentation).
	//! @param _action The action that should be processed
	//! @param _doc The document containing all the information
	virtual std::string processAction(const std::string& _action, ot::JsonDocument& _doc) override;

	//! @brief Will be called whenever a message should be processed. Core messages will be processed in the base and will not be forwarded to this function (see documentation).
	//! @param _sender The service that this message was sent from.
	//! @param _message The message that should be processed.
	//! @param _doc The document containing all the information.
	virtual std::string processMessage(ServiceBase* _sender, const std::string& _message, ot::JsonDocument& _doc) override;

	//! @brief Will be called when a UI connected to the session and is ready to work.
	virtual void uiConnected(ot::components::UiComponent* _ui) override;

	//! @brief Will be called when a UI disconnected from the session (is already closed).
	virtual void uiDisconnected(const ot::components::UiComponent* _ui) override;

	//! @brief Will be called when a model service connected to the session and is ready to work.
	virtual void modelConnected(ot::components::ModelComponent* _model) override;

	//! @brief Will be called when a mode service disconnected from the session (is already closed).
	virtual void modelDisconnected(const ot::components::ModelComponent* _model) override;

	//! @brief Will be called when a service is connected to the session (UI and model services will not trigger this function).
	//! @param _service The service information.
	virtual void serviceConnected(ot::ServiceBase* _service) override;

	//! @brief Will be called when a service is disconnected from the session (is already closed) (UI and model services will not trigger this function).
	//! @param _service The service information.
	virtual void serviceDisconnected(const ot::ServiceBase* _service) override;

	//! @brief Will be called before the whole session starts to shut down (shut down can not be stopped).
	//! At this point all services, that are listed as connected, are still reachable.
	virtual void preShutdown(void) override;

	//! @brief Will be called when the application is shutting down (shut down can not be stopped).
	virtual void shuttingDown(void) override;

	//! @brief Will return true if this application requires a relay service for a websocket connection.
	virtual bool startAsRelayService(void) const override;

	//! @brief Create settings that your application uses that are editable in the uiService.
	//! The created class will be deleted after used for sending or synchronizing with the database.
	//! The created settings will be requested upon Service startup to synchronize with the database,
	//! aswell as when the uiService is connected.
	virtual ot::PropertyGridCfg createSettings(void) const override;

	//! @brief This function will be called when the settings were synchronized with the database.
	//! At this point the values from the dataset should be stored since the dataset will be deleted after this function call.
	//! @param The dataset that contains all values.
	virtual void settingsSynchronized(const ot::PropertyGridCfg& _dataset) override;

	//! @brief This function will be called when the settings were changed in the uiService.
	//! The value of the provided item should be stored.
	//! If the change of the item will change the item visibility of any settings item, this function should return true,
	//! otherwise false. When returning true, the function createSettings() will be called and the created dataset will be
	//! send to the uiService to update the Settings in the dialog.
	//! @param The item that has been changed in the uiService (instance will be deleted after this function call).
	virtual bool settingChanged(const ot::Property* _item) override;

	SelectionHandler& getSelectionHandler() { return m_selectionHandler; }

	void addButtons();

	ViewVisualisationHandler& getVisualisationHandler() { return m_visualisationHandler; }

private:
	void queueAction(ActionType _type, const ot::JsonDocument& _document);

	void asyncActionWorker(void);

	void handleAsyncSelectionChanged(const ot::JsonDocument& _document);

	bool getContinueAsyncActionWorker(void);

	struct ActionData {
		ActionType type;
		std::string document;
	};

	std::mutex m_asyncActionMutex;
	std::list<ActionData> m_queuedActions;
	bool m_continueAsyncActionWorker;
	
	Model* m_model;
	SelectionHandler m_selectionHandler;
	MicroserviceNotifier* m_notifier;
	
	ActionAndFunctionHandler m_baseHandler;
	FileHandler m_fileHandler;
	ViewVisualisationHandler m_visualisationHandler;
	CopyPasteHandler m_copyPasteHandler;
	Application();
	~Application();
};
