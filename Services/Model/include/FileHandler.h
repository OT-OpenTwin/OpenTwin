// @otlicense
// File: FileHandler.h
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
#include "OTModelEntities/Visualization/IVisualisationText.h"
#include "OTModelEntities/Visualization/IVisualisationTable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/DataStruct/GenericDataStructMatrix.h"
#include "OTGui/Widgets/TableCfg.h"
#include "OTGui/Widgets/ToolBarButtonCfg.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTGuiAPI/TableActionHandler.h"
#include "OTGuiAPI/TextEditorActionHandler.h"
#include "OTCommunication/Handler/ActionHandler.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTModelEntities/EntityPythonManifest.h"
#include "OTModelEntities/Lms/UserLibraryElement.h"
#include "OTGui/Dialog/PropertyDialogCfg.h"

class FileHandler : public BusinessLogicHandler, public ot::TextEditorActionHandler, public ot::TableActionHandler
{
	OT_DECL_NOCOPY(FileHandler)
	OT_DECL_NOMOVE(FileHandler)
public:
	FileHandler();
	virtual ~FileHandler() = default;

	void addButtons(ot::components::UiComponent* _uiComponent);
	void handleOverwriteResponse(const std::string& _filePath, bool _overwrite);
	//! @brief Handles the export file to local library directory
	void handleExportFileToLocalLibraryDialog(const ot::PropertyDialogCfg& _dialogCfg);
	//! @brief Handles the export file to user library (directly into the user's database collection)
	void handleExportFileToUserLibraryDialog(const ot::PropertyDialogCfg& _dialogCfg);

	const std::string m_pythonScriptDialogGroup = "Python Script Export";
	const std::string m_circuitModelDialogGroup = "Circuit Model Export";
private:
	const std::string c_groupName = "File Imports";
	const std::string m_pythonUserCollectionName = "PythonScripts_User";
	const std::string m_circuitModelUserCollectionName = "CircuitData_User";
	const std::string m_environmentUserCollectionName = "Environments_User";
	ot::ToolBarButtonCfg m_buttonFileImport;
	ot::ToolBarButtonCfg m_buttonPythonImport;
	ot::ToolBarButtonCfg m_buttonExportFileToLibrary;
	ot::ToolBarButtonCfg m_buttonExportToUserLibrary;

	ot::UIDList m_entityIDsTopo;
	ot::UIDList m_entityVersionsTopo;
	ot::UIDList m_entityIDsData;
	ot::UIDList m_entityVersionsData;
	std::list<bool> m_forceVisible;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Button Handler

	ot::ButtonHandler m_buttonHandler;
	void handleImportTextFileButton();
	void handleImportPythonScriptButton();
	void handleExportFilesToLibrary();
	void handleExportToUserLibrary();

	//! @brief Shows a dialog to select a file for export to either local directory or user database collection.
	void showExportDialog(const std::string& _title, const std::string& _callbackAction);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action Handler

	ot::ActionHandler m_actionHandler;
	void handleImportTextFile(ot::JsonDocument& _document);
	void handleImportPythonScript(ot::JsonDocument& _document);
	virtual ot::ReturnMessage textEditorSaveRequested(const std::string& _entityName, const std::string& _text, size_t _nextChunkStartIndex) override;
	virtual ot::ReturnMessage tableSaveRequested(const ot::TableCfg& _cfg) override;
	virtual ot::ReturnMessage tableColumnFilterChanged(const ot::TableFilterChangeEvent& _event) override;

	//api @sercurity mTLS
	//api @action OT_ACTION_CMD_UI_RequestTextData
	//api @brief Request text data from a entity.
	//api The data is written to a GridFS file and the corresponding file info is returned.
	//api @param OT_ACTION_PARAM_MODEL_EntityName Name of the entity to request the text data from.
	//api @return Returns a ot::ReturnMessage containing the ot::GridFSFileInfo in JSON format in case of success.
	ot::ReturnMessage handleRequestTextData(ot::JsonDocument& _document);

	constexpr static std::string_view c_promptActionOverwriteFile = "Model.Prompt.OverwriteFile";
	void handleOverwriteFilePromptResponse(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Helper

	void importFile(const std::string& _fileMask, const std::string& _dialogTitle, const std::string& _functionName);
	//! @brief Stores the string as byte array. Content cannot be searched for but it is not necessary to guarantee UTF8 encoding
	void storeTextFile(ot::JsonDocument&& _doc, const std::string& _folderName);
	void addTextFilesToModel();
	void clearBuffer();
	
	void storeChangedText(ot::IVisualisationText* _entity, const std::string _text, size_t _nextChunkStartIndex);
	void storeChangedTable(ot::IVisualisationTable* _entity, const ot::TableCfg& _cfg);

	void processTableColumnFilterChanged(const ot::TableFilterChangeEvent& _event, ot::IVisualisationTable* _entity);

	void NotifyOwnerAsync(ot::JsonDocument&& _doc, const std::string _owner);
	//! @brief Filecontent is stored as binary, thus the encoding does not matter. The filename however is stored in properties and used in the visualisation. 
	//! Thus UTF8 encoding is required.
	void storeFileInDataBase(const std::string& _text, const std::string& _fileName, std::list<std::string>& _folderContent, const std::string& _folderName, const std::string& _fileFilter);

	// ###########################################################################################################################################################################################################################################################################################################################
	
	// Export to library functions

	//! @brief Struct to hold the entities related to an export operation together for easier handling
	struct DialogExportEntities {
		EntityFileText* circuitModel = nullptr;
		EntityFileText* circuitMetaFile = nullptr;
		EntityFileText* pythonScript = nullptr;
		EntityPythonManifest* pythonManifest = nullptr;
		EntityFileText* pythonMetaFile = nullptr;
		EntityFileText* manifestMetaFile = nullptr;
		bool isPythonExport = false;
		bool isCircuitExport = false;
	};

	//! @brief Stores information about pending file overwrites (content + metadata together)
	struct PendingFileOverwrite {
		std::string contentFilePath;
		std::string contentNewContent;
		std::string metaFilePath;
		std::string metaNewContent;
	};

	// Enum for file overwrite handling
	enum class FileOverwriteStatus {
		Skip,
		Write,
		PromptUser
	};


	//! @brief Load and validate all entities from dialog config
	DialogExportEntities loadDialogEntities(const ot::PropertyDialogCfg& _dialogCfg);

	//! @brief Load a single entity by path and perform type casting
	EntityBase* loadEntityByPath(const std::string& _entityPath, const std::string& _description);

	//! @brief Handle circuit export for both library and user library
	void handleCircuitExport(const DialogExportEntities& _entities, bool _exportToUserLibrary);

	//! @brief Handle Python export for both library and user library
	void handlePythonExport(const DialogExportEntities& _entities, bool _exportToUserLibrary);
	
	//! @brief Check for file overwrite and handle according to the status (skip, write, or prompt user)
	std::map<std::string, PendingFileOverwrite> m_pendingFileOverwrites;

	//! @brief Validates the metadata file by checking for required properties and correct formatting. Returns true if valid, false otherwise.
	bool validateMetaDataFile(EntityFileText* _metaFile);
	std::string getLibraryDataPath() const;
	bool ensureDirectoryExists(const std::string& _path) const;
	void writeFileToPath(const std::string& _filePath, const std::string& _content) const;

	//! @brief Checks if a file already exists at the given path and handles the potential overwrite according to the specified logic (skip, write, or prompt user). Returns the determined FileOverwriteStatus.
	FileOverwriteStatus checkAndHandleFileOverwrite(const std::string& _filePath, const std::string& _newContent, const std::string& _metaFilePath, const std::string& _metaNewContent) const;

	//! @brief Prompts the user with a choice to overwrite existing files when changes are detected. The pending file overwrite information is stored in m_pendingFileOverwrites and the user's response will be handled in handleOverwriteResponse.
	void promptUserForOverwrite(const std::string& _contentFilePath, const std::string& _metaFilePath, const std::string& _contentNewContent, const std::string& _metaNewContent) const;

	void exportPythonScriptsToLibrary(ot::UID _scriptID, ot::UID _manifestID, ot::UID _pythonMetaID, ot::UID _manifestMetaID, ot::UID _environmentID);

	void exportPythonManifest(EntityPythonManifest* _manifestEntity, EntityFileText* _metaEntity, const std::string& _basePath, ot::UID _environmentID);
	void exportPythonScript(EntityFileText* _scriptEntity, EntityFileText* _metaEntity, const std::string& _basePath, ot::UID _environmentID);
	void exportCircuitModel(EntityFileText* _modelEntity, EntityFileText* _metaEntity, const std::string& _basePath);

	// User export functions
	void createLibraryElementsForCircuitModel(EntityFileText* _modelEntity, EntityFileText* _metaEntity, std::list<ot::UserLibraryElement>& _elementsToExport);
	void createLibraryElementsForPythonScript(EntityFileText* _scriptEntity, EntityFileText* _metaEntity, std::list<ot::UserLibraryElement>& _elementsToExport, ot::UID _environmentID = ot::invalidUID);
	void createLibraryElementsForPythonManifest(EntityPythonManifest* _manifestEntity, EntityFileText* _metaEntity, std::list<ot::UserLibraryElement>& _elementsToExport);
	void exportLibraryElementsToUserLibrary(const std::list<ot::UserLibraryElement>& _elementsToExport);

	std::string createIncrementedPath(const std::string& _filePath);
	std::string ensureFileExtension(const std::string& _fileName, const std::string& _extension) const;
};
