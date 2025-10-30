// @otlicense
// File: EntityWithDynamicFields.cpp
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

#include "EntityWithDynamicFields.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <algorithm>
#include "BSONToVariableConverter.h"

static EntityFactoryRegistrar<EntityWithDynamicFields> registrar("EntityWithDynamicFields");

EntityWithDynamicFields::EntityWithDynamicFields(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, const std::string& owner)
	: EntityContainer(ID, parent, mdl, ms, owner)
{
	_bsonDocumentsByName["/"];
}

void EntityWithDynamicFields::InsertInField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName)
{
	if (_bsonDocumentsByName.find(documentName) == _bsonDocumentsByName.end())
	{
		if (documentName.size() == 0 || documentName[0] != '/')
		{
			documentName = "/" + documentName;
		}
		GenericBsonDocument newDocument;
		newDocument.setDocumentName(documentName);
		_bsonDocumentsByName.insert({ documentName, newDocument });
	}
	_bsonDocumentsByName[documentName].insertInDocumentField(fieldName, values);

	setModified();
}

void EntityWithDynamicFields::InsertInField(std::string fieldName, const std::list<ot::Variable>& values, std::string documentName)
{
	if (_bsonDocumentsByName.find(documentName) == _bsonDocumentsByName.end())
	{
		if (documentName.size() == 0 || documentName[0] != '/')
		{
			documentName = "/" + documentName;
		}
		GenericBsonDocument newDocument;
		newDocument.setDocumentName(documentName);
		_bsonDocumentsByName.insert({ documentName, newDocument });
	}
	_bsonDocumentsByName[documentName].insertInDocumentField(fieldName, values);

	setModified();
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
	if (documentName[0] != '/')
	{
		documentName = "/" + documentName;
	}
	if (_bsonDocumentsByName.find(documentName) == _bsonDocumentsByName.end())
	{
		throw std::exception(("Could not find document with the name " + documentName).c_str());
	}
	return &_bsonDocumentsByName[documentName];
}

const GenericDocument* EntityWithDynamicFields::getDocumentTopLevel()
{
	return getDocument("/");
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

void EntityWithDynamicFields::addStorageData(bsoncxx::builder::basic::document& storage)
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

	for (auto& document : _bsonDocumentsByName)
	{
		document.second.clearSubDocuments();
		documentNames.push_back(document.first);
	}

	auto it = documentNames.begin();
	
	//Go through all document names and link them to their parent.
	while (it != documentNames.end())
	{
		const std::string& currentDocumentName = *it;
		
		//Root document
		if ((currentDocumentName) == "/")
		{
			_bsonDocumentsByName[currentDocumentName].setDocumentName("DynamicFields");
		}
		else
		{
			std::string::difference_type hierarchyLevel = std::count((currentDocumentName).begin(), (currentDocumentName).end(), '/');

			std::string parentName;
			
			//Determine parent name
			if (hierarchyLevel == 1)
			{
				parentName = "/";
			}
			else
			{
				parentName = (*it).substr(0, (currentDocumentName).find_last_of('/'));
			}
			
			//Add all non existing ancestors
			std::string ancestorName =parentName;
			while(_bsonDocumentsByName.find(ancestorName) == _bsonDocumentsByName.end())
			{
				CreatePlainDocument(ancestorName);
				documentNames.push_back(ancestorName);
				if ((ancestorName).find_last_of('/') == 0)
				{
					ancestorName = "/";
				}
				else
				{
					ancestorName = (ancestorName).substr(0, (ancestorName).find_last_of('/'));
				}
			}
			
			//Add 
			std::string trimmedDocumentName = (currentDocumentName).substr((currentDocumentName).find_last_of('/') + 1, (currentDocumentName).size());
			_bsonDocumentsByName[(currentDocumentName)].setDocumentName(trimmedDocumentName);
			_bsonDocumentsByName[parentName].addSubDocument(&_bsonDocumentsByName[(currentDocumentName)]);
		}

		it++;
	}
}

void EntityWithDynamicFields::AddGenericDocumentToBsonDocument(const GenericBsonDocument* genericDocument, bsoncxx::builder::basic::document& bsonDocument)
{
	genericDocument->addAllFieldsToDocument(bsonDocument);

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
	auto type = element.type();
	const std::string fieldName(element.key().data());

	if (type == bsoncxx::type::k_document)
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
		BSONToVariableConverter converter;
		std::list<ot::Variable> values;
		if (type == bsoncxx::type::k_array)
		{
			auto arrayElements = element.get_array().value;
		
			auto firstElement = arrayElements.begin();
			auto arrayElementType = firstElement->type();

			for (const auto& arrayElement : arrayElements)
			{
				values.push_back(converter(arrayElement));
			}
			_bsonDocumentsByName[documentName].insertInDocumentField(fieldName, values);
		}
		else
		{
			std::list<ot::Variable> field{ converter(element) };
			_bsonDocumentsByName[documentName].insertInDocumentField(fieldName, field);
		}
	}

}