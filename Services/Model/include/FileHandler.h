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
	void handleExportDialog(const ot::PropertyDialogCfg& _dialogCfg);


	const std::string m_pythonScriptDialogGroup = "Python Script Export";
	const std::string m_circuitModelDialogGroup = "Circuit Model Export";
private:
	const std::string c_groupName = "File Imports";
	ot::ToolBarButtonCfg m_buttonFileImport;
	ot::ToolBarButtonCfg m_buttonPythonImport;
	ot::ToolBarButtonCfg m_buttonExportFileToLibrary;

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

	// Export to library functions

	bool validateMetaDataFile(EntityFileText* _metaFile);
	std::string getLibraryDataPath() const;
	bool ensureDirectoryExists(const std::string& _path) const;
	void writeFileToPath(const std::string& _filePath, const std::string& _content) const;

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

	std::map<std::string, PendingFileOverwrite> m_pendingFileOverwrites;

	FileOverwriteStatus checkAndHandleFileOverwrite(const std::string& _filePath, const std::string& _newContent, const std::string& _metaFilePath, const std::string& _metaNewContent) const;

	void promptUserForOverwrite(const std::string& _contentFilePath, const std::string& _metaFilePath, const std::string& _contentNewContent, const std::string& _metaNewContent) const;

	void exportCircuitModelsAsync(ot::UID _modelID, ot::UID _metaID);
	void exportFilesToLibraryAsync(ot::UID _scriptID, ot::UID _manifestID, ot::UID _pythonMetaID, ot::UID _manifestMetaID, ot::UID _environmentID);

	void exportPythonManifest(EntityPythonManifest* _manifestEntity, EntityFileText* _metaEntity, const std::string& _basePath, ot::UID _environmentID);
	void exportPythonScript(EntityFileText* _scriptEntity, EntityFileText* _metaEntity, const std::string& _basePath, ot::UID _environmentID);
	void exportCircuitModel(EntityFileText* _modelEntity, EntityFileText* _metaEntity, const std::string& _basePath);

	std::string createIncrementedPath(const std::string& _filePath);
	std::string ensureFileExtension(const std::string& _fileName, const std::string& _extension) const;
};
