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
#include "OTCore/CoreTypes.h"

#include <string>
#include <map>
#include <stdint.h>
#include <exception>
#include "OTCore/Variable.h"

class __declspec(dllexport) EntityWithDynamicFields : public EntityContainer
{
public:
	EntityWithDynamicFields(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);

	void InsertInField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName = "/");
	void InsertInField(std::string fieldName, const std::list<ot::Variable>& values, std::string documentName = "/");

	std::vector<std::string> getDocumentsNames(std::string parentDocument = "/") const;
	const GenericDocument* getDocument(std::string documentName);
	const GenericDocument* getDocumentTopLevel();
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
