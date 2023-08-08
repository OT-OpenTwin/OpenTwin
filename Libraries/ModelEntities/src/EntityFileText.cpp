#include "EntityFileText.h"

EntityFileText::EntityFileText(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner)
	: EntityFile(ID,parent,obs,ms,factory,owner)
{
}

void EntityFileText::setTextEncoding(ot::TextEncoding encoding)
{
	_encoding = encoding;
}

void EntityFileText::setSpecializedProperties()
{
	EntityPropertiesSelection::createProperty("Text Properties", "Text Encoding", { "ANSI", "UTF-8", "UTF-8 BOM", "UTF-16 BE BOM", "UTF-16 LE BOM", "Unknown" },"Unknown","default",getProperties());
}

void EntityFileText::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityFile::AddStorageData(storage);
	std::string encoding = "Unknown";
	if (_encoding == ot::TextEncoding::ANSI)
	{
		encoding = "ANSI";
	}
	else if (_encoding == ot::TextEncoding::UTF8)
	{
		encoding = "UTF8";
	}
	else if (_encoding == ot::TextEncoding::UTF8_BOM)
	{
		encoding = "UTF8BOM";
	}
	else if (_encoding == ot::TextEncoding::UTF16_BEBOM)
	{
		encoding = "UTF16BEBOM";
	}
	else if (_encoding == ot::TextEncoding::UTF16_LEBOM)
	{
		encoding = "UTF16LEBOM";
	}
	else
	{
		encoding = "Unknown";
	}
	storage.append(bsoncxx::builder::basic::kvp("TextEncoding", encoding));
}

void EntityFileText::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityFile::readSpecificDataFromDataBase(doc_view,entityMap);
	
	std::string encoding = doc_view["TextEncoding"].get_utf8().value.to_string();
	
	if (encoding == "ANSI")
	{
		encoding = ot::TextEncoding::ANSI;
	}
	else if (encoding == "UTF8")
	{
		_encoding = ot::TextEncoding::UTF8;
	}
	else if (encoding == "UTF8BOM")
	{
		_encoding = ot::TextEncoding::UTF8_BOM;
	}
	else if (encoding == "UTF16BEBOM")
	{
		_encoding = ot::TextEncoding::UTF16_BEBOM;
	}
	else if (encoding == "UTF16LEBOM")
	{
		_encoding = ot::TextEncoding::UTF16_LEBOM;
	}
	else
	{
		encoding = "Unknown";
	}
}
