// @otlicense

//! @file FileHandler.h
//! @authors Jan Wagner, Alexander Kuester
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
	virtual ot::ReturnMessage textEditorSaveRequested(const std::string& _entityName, const std::string& _text) override;
	virtual ot::ReturnMessage tableSaveRequested(const ot::TableCfg& _cfg) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Helper

	void importFile(const std::string& _fileMask, const std::string& _dialogTitle, const std::string& _functionName);
	//! @brief Stores the string as byte array. Content cannot be searched for but it is not necessary to guarantee UTF8 encoding
	void storeTextFile(ot::JsonDocument&& _doc, const std::string& _folderName);
	void addTextFilesToModel();
	void clearBuffer();
	
	void storeChangedText(IVisualisationText* _entity, const std::string _text);
	void storeChangedTable(IVisualisationTable* _entity, const ot::TableCfg& _cfg);
	void NotifyOwnerAsync(ot::JsonDocument&& _doc, const std::string _owner);
	//! @brief Filecontent is stored as binary, thus the encoding does not matter. The filename however is stored in properties and used in the visualisation. 
	//! Thus UTF8 encoding is required.
	void storeFileInDataBase(const std::string& _text, const std::string& _fileName, std::list<std::string>& _folderContent, const std::string& _folderName, const std::string& _fileFilter);
};
