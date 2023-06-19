#include "EntityWithDynamicFields.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <algorithm>

EntityWithDynamicFields::EntityWithDynamicFields(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, ClassFactory* factory, const std::string& owner)
	: EntityContainer(ID, parent, mdl, ms, factory, owner)
{
	_bsonDocumentsByName["/"];
}

std::vector<std::string> EntityWithDynamicFields::getDocumentsNames(std::string parentDocument) const
{
	std::vector<std::string> documentNames;
	documentNames.reserve(_bsonDocumentsByName.size());
	for (auto document : _bsonDocumentsByName)
	{
		if (document.first.find(parentDocument) != std::string::npos)
		{
			documentNames.push_back(document.first);
		}
	}
	return documentNames;
}

const GenericDocument* EntityWithDynamicFields::getDocument(std::string documentName)
{
	if (_bsonDocumentsByName.find(documentName) == _bsonDocumentsByName.end())
	{
		throw std::exception(("Could not find document with the name " + documentName).c_str());
	}
	return &_bsonDocumentsByName[documentName];
}

void EntityWithDynamicFields::ClearAllDocuments()
{
	_bsonDocumentsByName.clear();
	setModified();
}

void EntityWithDynamicFields::CreatePlainDocument(std::string documentName)
{
	if (_bsonDocumentsByName.find(documentName) == _bsonDocumentsByName.end())
	{
		if (documentName[0] != '/')
		{
			documentName = "/" + documentName;
		}
		_bsonDocumentsByName[documentName].setDocumentName(documentName);
	}
}

void EntityWithDynamicFields::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	OrderGenericDocumentsHierarchical();
	auto rootDocument = _bsonDocumentsByName["/"];
	bsoncxx::builder::basic::document dynamicFields;
	AddGenericDocumentToBsonDocument(&rootDocument, dynamicFields);
	storage.append(bsoncxx::builder::basic::kvp("DynamicFields", dynamicFields));
}

void EntityWithDynamicFields::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	_bsonDocumentsByName.insert({ "/", GenericBsonDocument() });
	_bsonDocumentsByName["/"].setDocumentName("DynamicFields");

	auto dynamicFields = doc_view["DynamicFields"].get_document();
	for (const auto& element : dynamicFields.view())
	{
		ExtractElementValues(element, "/");
	}
	OrderGenericDocumentsHierarchical();
}

void EntityWithDynamicFields::OrderGenericDocumentsHierarchical()
{

	std::list<std::string> documentNames;

	for (const auto& document : _bsonDocumentsByName)
	{
		documentNames.push_back(document.first);
	}

	auto it = documentNames.begin();
	while (it != documentNames.end())
	{
		std::string::difference_type hierarchyLevel = std::count((*it).begin(), (*it).end(), '/');

		if ((*it) == "/")
		{
			_bsonDocumentsByName[(*it)].setDocumentName("DynamicFields");
		}
		else
		{
			std::string subDocumentBase;
			if (hierarchyLevel == 1)
			{
				subDocumentBase = "/";
			}
			else
			{
				subDocumentBase = (*it).substr(0, (*it).find_last_of('/'));

			}
			if (_bsonDocumentsByName.find(subDocumentBase) == _bsonDocumentsByName.end())
			{
				throw std::exception(("Subdocument requires a parent document, which was not found: " + subDocumentBase).c_str());
			}
			std::string trimmedDocumentName = (*it).substr((*it).find_last_of('/') + 1, (*it).size());
			_bsonDocumentsByName[(*it)].setDocumentName(trimmedDocumentName);
			_bsonDocumentsByName[subDocumentBase].AddSubDocument(&_bsonDocumentsByName[(*it)]);
		}
		it++;
	}
}

void EntityWithDynamicFields::AddGenericDocumentToBsonDocument(const GenericBsonDocument* genericDocument, bsoncxx::builder::basic::document& bsonDocument)
{
	genericDocument->AddAllDocumentFieldsToDocument(bsonDocument);

	for (auto genericSubDocument : genericDocument->getSubDocuments())
	{
		bsoncxx::builder::basic::document bsonSubdocument;
		auto genericBsonSubdocument = dynamic_cast<const GenericBsonDocument*>(genericSubDocument);
		AddGenericDocumentToBsonDocument(genericBsonSubdocument, bsonSubdocument);
		std::string subDocName = genericSubDocument->getDocumentName();
		bsonDocument.append(kvp(subDocName, bsonSubdocument));
	}
}

void EntityWithDynamicFields::ExtractElementValues(const bsoncxx::document::element& element, std::string documentName)
{
	std::string fieldName = element.key().to_string();
	auto type = element.type();

	if (type == bsoncxx::type::k_array)
	{
		auto arrayElements = element.get_array().value;
		auto firstElement = arrayElements.begin();
		auto arrayElementType = firstElement->type();

		switch (arrayElementType)
		{
		case bsoncxx::type::k_double:
		{
			std::list<double> doubleFields;
			for (const auto& arrayElement : arrayElements)
			{
				doubleFields.push_back(arrayElement.get_double());
			}
			_bsonDocumentsByName[documentName].InsertInDocumentField(fieldName, doubleFields);
			break;
		}
		case bsoncxx::type::k_utf8:
		{
			std::list<std::string> stringList;
			for (const auto& arrayElement : arrayElements)
			{
				stringList.push_back(arrayElement.get_utf8().value.data());
			}
			_bsonDocumentsByName[documentName].InsertInDocumentField(fieldName, stringList);
			break;
		}
		case bsoncxx::type::k_int32:
		{
			std::list<int32_t> fields;
			for (const auto& arrayElement : arrayElements)
			{
				fields.push_back(arrayElement.get_int32());
			}
			_bsonDocumentsByName[documentName].InsertInDocumentField(fieldName, fields);
			break;
		}
		case bsoncxx::type::k_int64:
		{
			std::list<int64_t> fields;
			for (const auto& arrayElement : arrayElements)
			{
				fields.push_back(arrayElement.get_int64());
			}
			_bsonDocumentsByName[documentName].InsertInDocumentField(fieldName, fields);
			break;
		}
		default:
		{
			assert(0);
		}
		}
	}
	else if (type == bsoncxx::type::k_document)
	{
		for (const auto& subElement : element.get_document().view())
		{
			if (documentName == "/")
			{
				ExtractElementValues(subElement, documentName + fieldName);
			}
			else
			{
				ExtractElementValues(subElement, documentName + "/" + fieldName);
			}
		}
	}
	else
	{
		switch (type)
		{
		case bsoncxx::type::k_double:
		{
			std::list<double> field = { element.get_double() };
			_bsonDocumentsByName[documentName].InsertInDocumentField(fieldName, field);
			break;
		}
		case bsoncxx::type::k_utf8:
		{
			std::list<std::string> field = { element.get_utf8().value.data() };
			_bsonDocumentsByName[documentName].InsertInDocumentField(fieldName, field);
			break;
		}
		case bsoncxx::type::k_int32:
		{
			std::list<int32_t> field = { element.get_int32() };
			_bsonDocumentsByName[documentName].InsertInDocumentField(fieldName, field);
			break;
		}
		case bsoncxx::type::k_int64:
		{
			std::list<int64_t> field = { element.get_int64() };
			_bsonDocumentsByName[documentName].InsertInDocumentField(fieldName, field);
			break;
		}
		default:
		{
			assert(0);
		}
		}
	}
}