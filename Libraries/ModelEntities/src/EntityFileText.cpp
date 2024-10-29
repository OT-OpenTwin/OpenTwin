#include "EntityFileText.h"
#include <locale>
EntityFileText::EntityFileText(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	: EntityFile(ID,parent,obs,ms,factory,owner)
{
}

void EntityFileText::setTextEncoding(ot::TextEncoding::EncodingStandard encoding)
{
	_encoding = encoding;
	EntityPropertiesBase* base=	getProperties().getProperty("Text Encoding");
	if (base != nullptr)
	{
		auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(base);
		ot::TextEncoding encoding;
		selectionProperty->setValue(encoding.getString(_encoding));
		selectionProperty->setNeedsUpdate();
	}
	else
	{
		throw std::exception("Property \"Text Encoding\" could not be loaded");
	}
}

ot::TextEncoding::EncodingStandard EntityFileText::getTextEncoding() 
{
	EntityPropertiesBase* base = getProperties().getProperty("Text Encoding");
	if (base != nullptr)
	{
		auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(base);
		ot::TextEncoding encoding;
		return encoding.getType(selectionProperty->getValue());
	}
	else
	{
		return ot::TextEncoding::UNKNOWN;
	}
}

std::string EntityFileText::getText(void)
{
	const std::vector<char> plainData = getData()->getData();
	return std::string(plainData.begin(),plainData.end());
}

void EntityFileText::setText(const std::string& _text)
{
	auto dataEntity = getData();
	dataEntity->clearData();

	//Ensure correct encoding at this location!
	dataEntity->setData(_text.data(), _text.size());
	if (getModelState() != nullptr)
	{
		dataEntity->StoreToDataBase();
		getModelState()->addNewEntity(dataEntity->getEntityID(), this->getEntityID(), dataEntity->getEntityStorageVersion(), ModelStateEntity::tEntityType::DATA);
		setData(dataEntity->getEntityID(), dataEntity->getEntityStorageVersion());
	}
}

ot::TextEditorCfg EntityFileText::createConfig(void) {
	ot::TextEditorCfg result;
	result.setName(this->getName());
	result.setTitle(this->getName());
	result.setPlainText(this->getText());

	const std::string fileType = getFileType();
	if (fileType == "py")
	{
		result.setDocumentSyntax(ot::DocumentSyntax::PythonScript);
	}
	else
	{
		result.setDocumentSyntax(ot::DocumentSyntax::PlainText);
	}

	return result;
}

void EntityFileText::setSpecializedProperties()
{
	ot::TextEncoding encoding;
	EntityPropertiesSelection::createProperty("Text Properties", "Text Encoding", 
		{ 
			encoding.getString(ot::TextEncoding::ANSI),
			encoding.getString(ot::TextEncoding::UTF8),
			encoding.getString(ot::TextEncoding::UTF8_BOM),
			encoding.getString(ot::TextEncoding::UTF16_BEBOM),
			encoding.getString(ot::TextEncoding::UTF16_LEBOM),
			encoding.getString(ot::TextEncoding::UNKNOWN),
		},
		encoding.getString(_encoding),
		"default",getProperties());

	EntityPropertiesSelection::createProperty("Text Properties",
		"Syntax Highlight",
		ot::getAllSupportedDocumentSyntax(),
		ot::toString(ot::DocumentSyntax::PlainText),
		"default",
		this->getProperties()
		);
}

void EntityFileText::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityFile::AddStorageData(storage);
	ot::TextEncoding encoding;
	std::string encodingStr = encoding.getString(_encoding);
	storage.append(bsoncxx::builder::basic::kvp("TextEncoding", encodingStr));
}

void EntityFileText::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityFile::readSpecificDataFromDataBase(doc_view,entityMap);
	
	const std::string encodingStr(doc_view["TextEncoding"].get_utf8().value.data());
	ot::TextEncoding encoding;
	_encoding =	encoding.getType(encodingStr);
}
