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
// Service header
#include "MongoWrapper.h"

// OpenTwin header
#include "OTCore/ServiceBase.h"
#include "OTGui/Dialog/ModelLibraryDialogCfg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Handler/ActionHandler.h"
#include "OTModelEntities/Lms/LibraryElement.h"

// std header
//std Header
#include <string>
#include <thread>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <list>

class Application : public ot::ServiceBase {
	OT_DECL_ACTION_HANDLER(Application)
public:
	static Application& instance(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public functions
	int initialize(const char* _siteID, const char* _ownURL, const char* _globalSessionServiceURL, const char* _databasePWD);
	

private:

	//! @brief Enum for user library element existence status
	enum class LibraryElementExistenceStatus {
		NotExisting,
		ExistingWithIdenticalContent,
		ExistingWithSameContentButNewDependency,
		ExistingWithDifferentContent,
		Error
	};

	//! @brief Result struct for updateOrCreateLibraryElement
	//! Combines the existence status with an optional cached DB element (only set for ExistingWithIdenticalContent)
	struct LibraryElementCheckResult {
		LibraryElementExistenceStatus status = LibraryElementExistenceStatus::NotExisting;
		std::optional<std::shared_ptr<ot::LibraryElement>> existingElement;
	};

	//! @brief Prompt action constance
	const std::string c_promptActionOverwriteUserLibraryElement = "LMS.Prompt.OverwriteUserElement";

	//! @brief Helper function for user library element management
	void promptUserForLibraryElementOverwrite(const ot::UserLibraryElement& _element, std::list<ot::JsonDocument>& _remainingElements, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const std::string& _uiServiceUrl);
	void promptMessageToUI(const std::string& _message, const std::string& _uiServiceUrl);

	//! @brief Processes elements of an export request strictly one after another.
	//! If an element needs a user decision (overwrite prompt), processing of the remaining
	//! elements is deferred and resumed later from handleLibraryElementOverwritePromptResponse -
	//! with the previous element's FINAL (decided) LibraryElementID already known, so DependencyID
	//! chains are always correct, never based on a stale/pending ID.
	//! @param _remainingElements Not-yet-processed elements, in original order, as raw JSON.
	//! @param _previousElementId Final LibraryElementID of the element processed right before this
	//! call. std::nullopt for the very first element (no predecessor).
	void processNextLibraryElement(std::list<ot::JsonDocument> _remainingElements, std::optional<ot::UID> _previousElementId, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, const std::string& _uiServiceUrl);


	//! @brief Generate a unique element name by appending a suffix (_X) if the name already exists
	//! @return The unique name that doesn't exist in the database
	std::string generateUniqueElementName(const std::string& _baseName, const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);

	//! @brief Helper function to check if a library element with the same libraryElementId exists in the database and if so, increase the id by 1
	void ensureUniqueLibraryElementId(ot::LibraryElement& _element, const std::string& _collectionName, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);

	// Model library update functions
	bool launchModelLibraryUpdate(const std::string& _ownURL, const std::string& _databasePWD);

	//! @brief Helper function to retrieve library elements from local file system
	//! @param _modelFolderPath The path to the folder containing the model files
	//! @param _collectionName The name of the collection to which the library elements belong
	//! @return A list of library elements created from the model files in the specified folder. 
	std::list<ot::LibraryElement> getLocalModels(const std::string& _modelFolderPath, const std::string& _collectionName);

	//! @brief Helper function to fill a library element with the hash of the corresponding model file. 
	//! @param _element The library element to fill
	//! @param _modelFolderPath The path to the folder containing the model files
	void fillLibraryElementWithHash(ot::LibraryElement& _element, const std::string& _modelFolderPath);

	//! @brief Helper function to add model data to library elements by reading the corresponding model files from the specified folder path. 
	//! The function will match the file names with the library element file names and populate the data field of each library element accordingly.
	//! @param _elements List of library elements to which the data should be added. Each element's file name will be used to find the corresponding model file in the specified folder.
	//! @param _modelFolderPath The path to the folder containing the model files.
	void addDataToLibraryElements(std::list<std::shared_ptr<ot::LibraryElement>>& _elements, const std::string& _modelFolderPath);

	//! @brief Helper function to retrieve model information from the database based on the provided selection configuration and database credentials. The retrieved information is returned as a JSON string.
	//! @param _selectionCfg The library element selection configuration
	//! @param _dbUserName The database user name
	//! @param _dbUserPassword The database user password
	//! @param _dbServerUrl The database server URL
	//! @return The model information as a JSON string
	std::string getModelInformation(const ot::LibraryElementSelectionCfg& _selectionCfg,
		const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);

	//! @brief Helper function for user library element management
	//! @brief Checks existence of library element and removes elements that don't need updating from the list
	//! @param _elements List of library elements to check. Elements that already exist with identical content will be removed from the list.
	//! @param _dbUserName The database user name
	//! @param _dbUserPassword The database user password
	//! @param _dbServerUrl The database server URL
	//! @param _dependencyCheck If true, it will also check for dependencies of the library elements and their existence status. If false, it will only check the provided elements without considering dependencies.
	//! @return The existence status of the library element
	LibraryElementCheckResult updateOrCreateLibraryElement(std::list<std::shared_ptr<ot::LibraryElement>>& _elements, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, bool _dependencyCheck = true);

	//! @brief Helper function to add library elements to the database
	//! @param _elements The library elements to add
	//! @param _dbUserName The database user name
	//! @param _dbUserPassword The database user password
	//! @param _dbServerUrl The database server URL
	//! @param _allowUpdate If true, an existing element with the same name will be updated in place
	//! instead of inserting a new document. If false (default), a new document is always inserted,
	//! even if an element with the same name already exists.
	void addLibraryElement(std::list<std::shared_ptr<ot::LibraryElement>>& _elements, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl, bool _allowUpdate = false);

	//! @brief Helper function to create a ModelLibraryDialogCfg based on the provided selection configuration and database credentials
	//! @param _selectionCfg The library element selection configuration
	//! @param _dbUserName The database user name
	//! @param _dbUserPassword The database user password
	//! @param _dbServerUrl The database server URL
	//!	@return An optional ModelLibraryDialogCfg. It will contain a value if the configuration was created successfully, or std::nullopt if an error occurred (e.g., database connection failure).
	std::optional<ot::ModelLibraryDialogCfg> createModelLibraryDialogCfg(const ot::LibraryElementSelectionCfg _selectionCfg, const std::string& _dbUserName, const std::string& _dbUserPassword, const std::string& _dbServerUrl);


	//! @brief Helper function to process library documents and convert them to a ModelLibraryDialogCfg
	//! @param _documents The library documents to process
	//! @param _dialogCfg The dialog configuration to populate
	void processLibraryDocuments(const ot::ConstJsonArray& _documents, ot::ModelLibraryDialogCfg& _dialogCfg, const std::string& _collectionName ,bool isCustomElement);

	std::string sendConfigToUI(const ot::JsonDocument& _doc, const std::string& _uiUrl);
	std::string sendMessageToModel(const ot::JsonDocument& _doc, const std::string& _modelUrl);
	std::string sendAsyncMessageToModel(const ot::JsonDocument& _doc, const std::string& _modelUrl);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler
	OT_HANDLER(handleGetCompleteSelectedDocument, Application, OT_ACTION_CMD_LMS_GetDocument, ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleCreateDialogConfig, Application, OT_ACTION_CMD_LMS_CreateConfig, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleModelDialogConfirmed, Application, OT_ACTION_CMD_UI_ModelDialogConfirmed, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleModelDialogCanceled, Application, OT_ACTION_CMD_UI_ModelDialogCanceled, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleLibraryElementRequest, Application, OT_ACTION_CMD_LMS_LibraryElementRequest, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleAddUserLibraryElement, Application, OT_ACTION_CMD_LMS_AddUserLibraryElement, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleLibraryElementOverwritePromptResponse, Application, c_promptActionOverwriteUserLibraryElement, ot::SECURE_MESSAGE_TYPES)

	// ###########################################################################################################################################################################################################################################################################################################################
	
	MongoWrapper db;
	// Constructor/Destructor

	Application();
	virtual ~Application();	
};
