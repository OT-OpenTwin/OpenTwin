#pragma once

// OpenTwin header
#include "OTServiceFoundation/UiComponent.h"
#include "ActionAndFunctionHandler.h"
#include "IVisualisationText.h"
#include "IVisualisationTable.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTGui/TableCfg.h"
#include "OTGui/ToolBarButtonCfg.h"
#include "OTCore/GenericDataStructMatrix.h"

class FileHandler : public ActionAndFunctionHandler, public BusinessLogicHandler
{
public:
	FileHandler() = default;
	virtual ~FileHandler() = default;

	FileHandler(const FileHandler& _other) = delete;
	FileHandler(FileHandler&& _other) = delete;
	FileHandler& operator=(const FileHandler& _other) = delete;
	FileHandler& operator=(FileHandler&& _other) = delete;

	void addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName);

protected:
	virtual bool handleAction(const std::string& _action, ot::JsonDocument& _doc) override;

private:
	ot::ToolBarButtonCfg m_buttonFileImport;
	ot::ToolBarButtonCfg m_buttonPythonImport;

	ot::UIDList m_entityIDsTopo;
	ot::UIDList m_entityVersionsTopo;
	ot::UIDList m_entityIDsData;
	ot::UIDList m_entityVersionsData;
	std::list<bool> m_forceVisible;
	
	void importFile(const std::string& _fileMask, const std::string& _dialogTitle, const std::string& _functionName);
	//! @brief Stores the string as byte array. Content cannot be searched for but it is not necessary to guarantee UTF8 encoding
	void storeTextFile(ot::JsonDocument&& _doc, const std::string& _folderName);
	void addTextFilesToModel();
	void clearBuffer();
	
	void handleChangedText(ot::JsonDocument& _doc);
	void handleChangedTable(ot::JsonDocument& _doc);
	void storeChangedText(IVisualisationText* _entity, const std::string _text);
	void storeChangedTable(IVisualisationTable* _entity ,ot::TableCfg& _cfg);
	void NotifyOwnerAsync(ot::JsonDocument&& _doc, const std::string _owner);
	//! @brief Filecontent is stored as binary, thus the encoding does not matter. The filename however is stored in properties and used in the visualisation. 
	//! Thus UTF8 encoding is required.
	void storeFileInDataBase(const std::string& _text, const std::string& _fileName, std::list<std::string>& _folderContent, const std::string& _folderName, const std::string& _fileFilter);
};
