/*****************************************************************//**
 * \file   EntityWithDynamicFields.h
 * \brief  Abstract class that allows assembling fields and subdocuments of the entity during runtime.
 *
 * \author Wagner
 * \date   April 2023
 *********************************************************************/
#pragma once
#include "EntityContainer.h"
#include "GenericBsonDocument.h"
#include "OpenTwinCore/CoreTypes.h"

#include <string>
#include <map>
#include <stdint.h>
#include <exception>
#include <type_traits>

class __declspec(dllexport) EntityWithDynamicFields : public EntityContainer
{
public:
	EntityWithDynamicFields(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, ClassFactory* factory, const std::string& owner);

	template <class T>
	void InsertInField(std::string fieldName, std::list<T> values, std::string documentName = "/");

	std::vector<std::string> getDocumentsNames(std::string parentDocument = "/") const;
	const GenericDocument* getDocument(std::string documentName);
	void ClearAllDocuments();

protected:
	void CreatePlainDocument(std::string documentName);
	virtual void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	std::map<std::string, GenericBsonDocument> _bsonDocumentsByName;
	void OrderGenericDocumentsHierarchical();
	void AddGenericDocumentToBsonDocument(const GenericBsonDocument* genericDocument, bsoncxx::builder::basic::document& bsonDocument);
	void ExtractElementValues(const bsoncxx::document::element& element, std::string documentName);
};

template <class T>
void EntityWithDynamicFields::InsertInField(std::string fieldName, std::list<T> values, std::string documentName)
{
	static_assert(std::is_same<T, int32_t>::value || std::is_same<T, int64_t>::value || std::is_same<T, double>::value || std::is_same<T, std::string>::value,
		"Function template only supports int32, int64, double and string.");

	if (_bsonDocumentsByName.find(documentName) == _bsonDocumentsByName.end())
	{
		if (documentName[0] != '/')
		{
			documentName = "/" + documentName;
		}
		GenericBsonDocument newDocument;
		newDocument.setDocumentName(documentName);
		_bsonDocumentsByName.insert({ documentName, newDocument });
	}
	_bsonDocumentsByName[documentName].InsertInDocumentField(fieldName, values);
	setModified();
};
