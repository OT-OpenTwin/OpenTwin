#include "stdafx.h"
#include "FileHandler.h"
#include "OTCommunication/ActionTypes.h"
#include "Application.h"
#include "OTServiceFoundation/Encryption.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTCore/EncodingGuesser.h"
#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"
#include "OTCore/EncodingConverter_UTF16ToUTF8.h"
#include "EntityFileText.h"
#include "Model.h"
#include "OTCore/FolderNames.h"
#include <assert.h>
#include "QueuingHttpRequestsRAII.h"
#include "QueuingDatabaseWritingRAII.h"

void FileHandler::addButtons(ot::components::UiComponent* _uiComponent, const std::string& _pageName)
{
	const std::string groupName = "File Imports";

	_uiComponent->addMenuGroup(_pageName,groupName);
	m_buttonFileImport.SetDescription(_pageName, groupName, "Import Text File");
	_uiComponent->addMenuButton(m_buttonFileImport, ot::LockModelWrite, "TextVisible");
}

bool FileHandler::handleAction(const std::string& _action, ot::JsonDocument& _doc)
{
	bool actionIsHandled= false;
	const std::string buttonDescr = m_buttonFileImport.GetFullDescription();
	if (_action == buttonDescr)
	{
		importFile("Text files (*.csv;*.txt)","Import Text File","ImportTextFile");
		actionIsHandled = true;
	}
	else if (_action == "ImportTextFile")
	{
		storeTextFile(_doc);
		actionIsHandled = true;
	}
	else
	{
		actionIsHandled = false;
	}
	return actionIsHandled;
}

void FileHandler::importFile(const std::string& _fileMask, const std::string& _dialogTitle, const std::string& _functionName)
{
	const std::string& serviceURL = Application::instance()->serviceURL();
	const std::string serviceName =	Application::instance()->serviceName();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString(_dialogTitle, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(_fileMask, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(_functionName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent, ot::JsonValue(true), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_LoadMultiple, ot::JsonValue(true), doc.GetAllocator());
	std::string response;
	Application::instance()->uiComponent()->sendMessage(false, doc, response);
}


void FileHandler::storeTextFile(ot::JsonDocument& _document)
{
	std::list<std::string> contents = ot::json::getStringList(_document, OT_ACTION_PARAM_FILE_Content);
	std::list<int64_t> 	uncompressedDataLengths = ot::json::getInt64List(_document, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);
	std::list<std::string> fileNames = ot::json::getStringList(_document, OT_ACTION_PARAM_FILE_OriginalName);
	assert(fileNames.size() == contents.size() && contents.size() == uncompressedDataLengths.size());

	{
		QueuingDatabaseWritingRAII queueDatabase;
		auto uncompressedDataLength = uncompressedDataLengths.begin();
		auto content = contents.begin();
		for (std::string& fileName : fileNames)
		{
			std::string fileContent = ot::decryptAndUnzipString(*content, *uncompressedDataLength);
			ensureUTF8Encoding(fileName);
			storeFileInDataBase(fileContent, fileName);
			uncompressedDataLength++;
			content++;
		}
	}

	addTextFilesToModel();
}

void FileHandler::ensureUTF8Encoding(std::string& _text)
{
	ot::EncodingGuesser guesser;
	ot::TextEncoding::EncodingStandard encodingStandard = guesser(_text.data(),_text.size());
	if (encodingStandard == ot::TextEncoding::EncodingStandard::ANSI)
	{
		ot::EncodingConverter_ISO88591ToUTF8 converter;
		converter(_text);
	}
	else if (encodingStandard == ot::TextEncoding::EncodingStandard::UTF16_BEBOM || encodingStandard == ot::TextEncoding::EncodingStandard::UTF16_LEBOM)
	{
		ot::EncodingConverter_UTF16ToUTF8 converter;
		converter(encodingStandard,_text);
	}
	else if (encodingStandard == ot::TextEncoding::EncodingStandard::UNKNOWN)
	{
		throw std::exception("File could not be imported, because the text encoding could not be determined.");
	}
	else
	{
		assert(encodingStandard == ot::TextEncoding::EncodingStandard::UTF8 || encodingStandard == ot::TextEncoding::EncodingStandard::UTF8_BOM);
	}
}

void FileHandler::storeFileInDataBase(const std::string& _text, const std::string& _fileName)
{
	Model* model = Application::instance()->getModel();
	ot::UID entIDData =	model->createEntityUID();
	ot::UID entIDTopo =	model->createEntityUID();
	const std::string serviceName = Application::instance()->serviceName();
	EntityFileText newText(entIDTopo, nullptr, nullptr, nullptr, nullptr, serviceName);	

	EntityBinaryData fileContent(entIDData, &newText, nullptr, nullptr, nullptr,serviceName);
	fileContent.setData(_text.data(), _text.size());
	fileContent.StoreToDataBase();

	newText.setData(fileContent.getEntityID(), fileContent.getEntityStorageVersion());

	size_t fileNamePos = _fileName.find_last_of("/");
	std::string path = _fileName.substr(0, fileNamePos);
	std::string name = _fileName.substr(fileNamePos+1);
	std::string type = name.substr(name.find_last_of('.') + 1);
	ot::EncodingGuesser guesser;
	newText.setFileProperties(path, name,type);
	newText.setName( ot::FolderNames::FilesFolder + "/" + name);
	
	newText.setTextEncoding(guesser(_text.data(), _text.size()));
	newText.StoreToDataBase();
	m_entityIDsTopo.push_back(entIDTopo);
	m_entityVersionsTopo.push_back(newText.getEntityStorageVersion());
	m_entityIDsData.push_back(entIDData);
	m_entityVersionsData.push_back(fileContent.getEntityStorageVersion());
	m_forceVisible.push_back(false);
}

void FileHandler::addTextFilesToModel()
{
	QueuingHttpRequestsRAII wrapper;
	Model* model = Application::instance()->getModel();
	model->addEntitiesToModel(m_entityIDsTopo, m_entityVersionsTopo, m_forceVisible, m_entityIDsData, m_entityVersionsData, m_entityIDsTopo, "Added File", true, false);
}