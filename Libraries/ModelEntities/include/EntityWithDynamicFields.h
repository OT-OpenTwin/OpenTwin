// @otlicense
// File: EntityWithDynamicFields.h
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

#pragma once
#include "EntityContainer.h"
#include "GenericBsonDocument.h"
#include "OTCore/CoreTypes.h"

#include <string>
#include <map>
#include <stdint.h>
#include <exception>
#include "OTCore/Variable.h"

//! @brief Abstract class that allows assembling fields and subdocuments of the entity during runtime.
class __declspec(dllexport) EntityWithDynamicFields : public EntityContainer
{
	friend class FixtureEntityWithDynamicFields;
public:
	EntityWithDynamicFields() : EntityWithDynamicFields(0, nullptr, nullptr, nullptr, "") {};
	EntityWithDynamicFields(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms, const std::string& owner);

	void InsertInField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName = "/");
	void InsertInField(std::string fieldName, const std::list<ot::Variable>& values, std::string documentName = "/");

	std::vector<std::string> getDocumentsNames(std::string parentDocument = "/") const;
	const GenericDocument* getDocument(std::string documentName);
	const GenericDocument* getDocumentTopLevel();
	void ClearAllDocuments();

protected:
	void CreatePlainDocument(std::string documentName);
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	std::map<std::string, GenericBsonDocument> _bsonDocumentsByName;
	void OrderGenericDocumentsHierarchical();
	void AddGenericDocumentToBsonDocument(const GenericBsonDocument* genericDocument, bsoncxx::builder::basic::document& bsonDocument);
	void ExtractElementValues(const bsoncxx::document::element& element, std::string documentName);
};
