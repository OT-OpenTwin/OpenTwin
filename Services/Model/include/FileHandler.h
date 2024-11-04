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

	void importFile(const std::string& _fileMask, const std::string& _dialogTitle, const std::string& _functionName);
	void storeTextFile(ot::JsonDocument& _doc);
	void ensureUTF8Encoding(std::string& _text);
	void storeFileInDataBase(const std::string& _text, const std::string& _fileName);
};
