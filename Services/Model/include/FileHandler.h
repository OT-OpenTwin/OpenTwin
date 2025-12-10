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
#include "IVisualisationText.h"
#include "IVisualisationTable.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTGui/TableCfg.h"
#include "OTGui/ToolBarButtonCfg.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTGuiAPI/TableActionHandler.h"
#include "OTGuiAPI/TextEditorActionHandler.h"
#include "OTCommunication/ActionHandler.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"

class FileHandler : public BusinessLogicHandler, public ot::TextEditorActionHandler, public ot::TableActionHandler
{
	OT_DECL_NOCOPY(FileHandler)
	OT_DECL_NOMOVE(FileHandler)
public:
	FileHandler();
	virtual ~FileHandler() = default;

	void addButtons(ot::components::UiComponent* _uiComponent);

private:
	const std::string c_groupName = "File Imports";
	ot::ToolBarButtonCfg m_buttonFileImport;
	ot::ToolBarButtonCfg m_buttonPythonImport;

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

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action Handler

	ot::ActionHandler m_actionHandler;
	void handleImportTextFile(ot::JsonDocument& _document);
	void handleImportPythonScript(ot::JsonDocument& _document);
	virtual ot::ReturnMessage textEditorSaveRequested(const std::string& _entityName, const std::string& _text, size_t _nextChunkStartIndex) override;
	virtual ot::ReturnMessage tableSaveRequested(const ot::TableCfg& _cfg) override;

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
	
	void storeChangedText(IVisualisationText* _entity, const std::string _text, size_t _nextChunkStartIndex);
	void storeChangedTable(IVisualisationTable* _entity, const ot::TableCfg& _cfg);
	void NotifyOwnerAsync(ot::JsonDocument&& _doc, const std::string _owner);
	//! @brief Filecontent is stored as binary, thus the encoding does not matter. The filename however is stored in properties and used in the visualisation. 
	//! Thus UTF8 encoding is required.
	void storeFileInDataBase(const std::string& _text, const std::string& _fileName, std::list<std::string>& _folderContent, const std::string& _folderName, const std::string& _fileFilter);
};
