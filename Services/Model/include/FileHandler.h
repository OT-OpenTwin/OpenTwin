#pragma once
#include "OTServiceFoundation/MenuButtonDescription.h"
#include "OTServiceFoundation/UiComponent.h"
#include "ActionHandler.h"

class FileHandler : public ActionAndFunctionHandler
{
public:
	FileHandler() = default;
	~FileHandler() = default;

	FileHandler(const FileHandler& _other) = delete;
	FileHandler(FileHandler&& _other) = delete;
	FileHandler& operator=(const FileHandler& _other) = delete;
	FileHandler& operator=(FileHandler&& _other) = delete;

	void addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName);
	virtual bool handleAction(const std::string& _action, ot::JsonDocument& _doc) override;

private:
	ot::MenuButtonDescription m_buttonFileImport;
	ot::UIDList m_entityIDsTopo;
	ot::UIDList m_entityVersionsTopo;
	ot::UIDList m_entityIDsData;
	ot::UIDList m_entityVersionsData;
	std::list<bool> m_forceVisible;
	
	void importFile(const std::string& _fileMask, const std::string& _dialogTitle, const std::string& _functionName);
	void storeTextFile(ot::JsonDocument& _doc);
	void addTextFilesToModel();
	void ensureUTF8Encoding(std::string& _text);

	//! @brief Filecontent is stored as binary, thus the encoding does not matter. The filename however is stored in properties and used in the visualisation. 
	//! Thus UTF8 encoding is required.
	void storeFileInDataBase(const std::string& _text, const std::string& _fileName);
};
