// @otlicense
// File: Application.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTServiceFoundation/ApplicationBase.h"
#include "FileHandler.h"
#include "PlotHandler.h"
#include "BlockHandler.h"
#include "MaterialHandler.h"
#include "SelectionHandler.h"
#include "CopyPasteHandler.h"
#include "ViewVisualisationHandler.h"
#include "LibraryManagementWrapper.h"
#include "ProjectInformationHandler.h"

// std header
#include <list>
#include <mutex>

class Model;
class MicroserviceNotifier;

class Application : public ot::ApplicationBase, public ot::ActionHandler {
public:
	enum class ActionType {
		SelectionChanged
	};

	static Application* instance();
	
	static std::string getToolBarPageName() { return "Model"; };

	// ##################################################################################################################################################################################################################

	// Action handler

	//api @action OT_ACTION_CMD_PROJ_Save
	//api @brief Saves a project in the database.
	//api @security mTLS
	//api @return In case of success will return an empty String.
	//api In case of error will return a String with prefix "Error: ".
	void handleProjectSave();

//	//api @security mTLS
	ot::ReturnMessage handleCheckProjectOpen();

	//api @action OT_ACTION_CMD_MODEL_SelectionChanged
	//api @brief Changes a selection.
	//api @security mTLS
	//api @return An empty String. 
	void handleSelectionChanged(ot::JsonDocument& _document);

	//api @action OT_ACTION_CMD_MODEL_ItemRenamed
	//api @brief Renames a model item.
	//api @security mTLS
	//api @param OT_ACTION_PARAM_MODEL_ID Unsigned Integer 64 Model ID (if model exists)
	//api @param OT_ACTION_PARAM_MODEL_ITM_Name String New name (if model exists)
	//api @return In case of success will return an empty String.
	//api In case of error will return a String with prefix "Error: ".
	void handleItemRenamed(ot::JsonDocument& _document);

	//api @action OT_ACTION_CMD_SetVisualizationModel
	//api @brief Visualizes a model.
	//api Opens a project and visualizes a model.
	//api @security mTLS
	//api @param OT_ACTION_PARAM_MODEL_ID Unsigned Integer 64 Model ID (if model exists)
	//api @param OT_ACTION_PARAM_VIEW_ID Unsigned Integer 64 View ID
	//api @return In case of success will return an empty String.
	//api In case of error will return a String with prefix "Error: ".
	void handleSetVisualizationModel(ot::JsonDocument& _document);

	//api @action OT_ACTION_CMD_GetVisualizationModel
	//api @brief Gets the visualization Model ID.
	//api @security mTLS
	//api @rparam OT_ACTION_PARAM_BASETYPE_UID Unsigned Integer 64 Visualization Model ID (if successful)
	//api @return In case of error will return a String with prefix "Error: ".
	std::string handleGetVisualizationModel(ot::JsonDocument& _document);

	//api @action OT_ACTION_CMD_MODEL_GetIsModified
	//api @brief Checks if the model has been modified.
	//api @security mTLS
	//api @rparam OT_ACTION_PARAM_BASETYPE_Bool Boolean Is model modified (if successful)
	//api @return In case of error will return a String with prefix "Error: ".
	std::string handleGetIsModified(ot::JsonDocument& _document);

	std::string handleGenerateEntityIDs(ot::JsonDocument& _document);

	void handleRequestImportTableFile(ot::JsonDocument& _document);

	void handleQueueMessages(ot::JsonDocument& _document);

	std::string handleGetListOfFolderItems(ot::JsonDocument& _document);

	std::string handleGetIDsOfFolderItemsByType(ot::JsonDocument& _document);

	void handleUpdateVisualizationEntity(ot::JsonDocument& _document);

	void handleUpdateGeometryEntity(ot::JsonDocument& _document);

	void handleModelChangeOperationCompleted(ot::JsonDocument& _document);

	void handleRequestUpdateVisualizationEntity(ot::JsonDocument& _document);

	std::string handleCheckParentUpdates(ot::JsonDocument& _document);

	void handleAddEntities(ot::JsonDocument& _document);

	void handleUpdateTopologyEntity(ot::JsonDocument& _document);

	void handleAddGeometryOperation(ot::JsonDocument& _document);

	void handleDeleteEntity(ot::JsonDocument& _document);

	void handleMeshingCompleted(ot::JsonDocument& _document);

	std::string handleGetEntityInformationByID(ot::JsonDocument& _document);

	std::string handleGetEntityInformationByName(ot::JsonDocument& _document);

	std::string handleGetSelectedEntityInformation(ot::JsonDocument& _document);

	std::string handleGetEntityChildInformationByName(ot::JsonDocument& _document);

	std::string handleGetEntityChildInformationByID(ot::JsonDocument& _document);

	std::string handleGetAllGeometryEntitiesForMeshing(ot::JsonDocument& _document);

	std::string handleGetCurrentVisualizationModelID();

	void handleEntitiesSelected(ot::JsonDocument& _document);

	std::string handleGetEntityIdentifier(ot::JsonDocument& _document);

	std::string handleGetEntitiesFromAnotherCollection(ot::JsonDocument& _document);

	std::string handleViewsFromProjectType();

	//api @action OT_ACTION_CMD_MODEL_RequestVisualisationData
	//api @brief Shows a data view in the UI.
	//api @security mTLS
	//api @param OT_ACTION_PARAM_MODEL_EntityID Unsigned Integer 64 Entity ID
	//api @param OT_ACTION_PARAM_Visualisation_Config Object Visualisation configuration
	//api @return A String containing an ot::ReturnMessage.
	ot::ReturnMessage handleVisualisationDataRequest(ot::JsonDocument& _document);

	void handleImportTableFile(ot::JsonDocument& _document);

	// ##################################################################################################################################################################################################################

	// Action handler: Properties

	void handleSetPropertiesFromJSON(ot::JsonDocument& _document);

	std::string handleGetEntityProperties(ot::JsonDocument& _document);

	std::string handleGetEntityPropertiesByName(ot::JsonDocument& _document);

	void handleUpdatePropertyGrid(ot::JsonDocument& _document);

	void handleAddPropertiesToEntities(ot::JsonDocument& _document);

	void handleUpdatePropertiesOfEntities(ot::JsonDocument& _document);

	void handleDeleteProperty(ot::JsonDocument& _document);

	// ##################################################################################################################################################################################################################

	// Action handler: Versions

	std::string handleGetCurrentVersion();

	void handleActivateVersion(ot::JsonDocument& _document);

	void handleVersionSelected(ot::JsonDocument& _document);

	void handleVersionDeselected(ot::JsonDocument& _document);

	void handleSetVersionLabel(ot::JsonDocument& _document);

	// ##################################################################################################################################################################################################################

	// Action handler: UI callbacks

	void handlePromptResponse(ot::JsonDocument& _document);

	//api @action OT_ACTION_CMD_UI_TABLE_Setup
	//api @brief Shows a table view in the UI.
	//api @security mTLS
	//api @param OT_ACTION_PARAM_NAME String Table name
	//api @param OT_ACTION_PARAM_Visualisation_Config Object Visualisation configuration
	//api @return A String containing an ot::ReturnMessage.
	ot::ReturnMessage handleShowTable(ot::JsonDocument& _document);

	void handleModelDialogConfirmed(ot::JsonDocument& _document);

	void handleModelDialogCanceled(ot::JsonDocument& _document);

	// ##################################################################################################################################################################################################################

	// Setter / Getter

	Model* getModel() const { return m_model; };
	MicroserviceNotifier* getNotifier() const { return m_notifier; };

	std::string getEntityInformation(std::list<ot::UID>& _entityIDList) const;

	// ##################################################################################################################################################################################################################

	// Communication

	bool queuedRequestToFrontend(const ot::JsonDocument& _request);
	bool queuedRequestToFrontend(const ot::JsonDocument& _request, std::list<std::pair<ot::UID, ot::UID>>& _prefetchIDs);
	void flushRequestsToFrontEnd();

	// ##################################################################################################################################################################################################################

	// Required functions

	virtual void initialize() override;

	//! @brief Will be called when a UI connected to the session and is ready to work.
	virtual void uiConnected(ot::components::UiComponent* _ui) override;

	//! @brief Will be called before the whole session starts to shut down (shut down can not be stopped).
	//! At this point all services, that are listed as connected, are still reachable.
	virtual void preShutdown() override;

	//! @brief Will be called when the application is shutting down (shut down can not be stopped).
	virtual void shuttingDown() override;

	SelectionHandler& getSelectionHandler() { return m_selectionHandler; }

	void addButtons();

	ViewVisualisationHandler& getVisualisationHandler() { return m_visualisationHandler; }
	LibraryManagementWrapper& getLibraryManagementWrapper() { return m_libraryManagementWrapper	; }
	BlockHandler& getBlockHandler() { return m_blockHandler; }

	//! @brief Stores the given content in a temporary file and returns the path to that file.
	//! @param _content The content to store in the temporary file.
	//! @param _uncompressedDataLength The length of the uncompressed data.
	//! @return The path to the temporary file.
	std::string storeTemporaryFile(const std::string& _content, uint64_t _uncompressedDataLength);

private:
	void queueAction(ActionType _type, const ot::JsonDocument& _document);

	void asyncActionWorker();

	void handleAsyncSelectionChanged(const ot::JsonDocument& _document);

	bool getContinueAsyncActionWorker();

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
	
	ProjectInformationHandler m_projectInformationHandler;
	FileHandler m_fileHandler;
	PlotHandler m_plotHandler;
	MaterialHandler m_materialHandler;
	ViewVisualisationHandler m_visualisationHandler;
	CopyPasteHandler m_copyPasteHandler;
	BlockHandler m_blockHandler;
	LibraryManagementWrapper m_libraryManagementWrapper;

	Application();
	~Application();
};
