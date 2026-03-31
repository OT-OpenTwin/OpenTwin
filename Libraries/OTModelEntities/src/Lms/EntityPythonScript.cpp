// @otlicense
// File: EntityPythonScript.cpp
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

#include "OTModelEntities/Lms/EntityPythonScript.h"
#include "OTModelEntities/Lms/LibraryElementRequest.h"
#include "OTModelEntities/EntityPythonManifest.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCore/FolderNames.h"
#include "OTCore/EncodingGuesser.h"

static EntityFactoryRegistrar<EntityPythonScript> registrar(EntityPythonScript::className());

void EntityPythonScript::setLibraryElement(const ot::LibraryElement& _libraryElement, ot::NewModelStateInfo& _modelState) {
	std::string newEntityFolder = _libraryElement.getNewEntityFolder();
	std::string newEntityName = _libraryElement.getName();

	std::string fileName = newEntityFolder + "/" + newEntityName;
	size_t fileNamePos = fileName.find_last_of("/");
	std::string path = fileName.substr(0, fileNamePos);
	std::string name = fileName.substr(fileNamePos + 1);
	std::string type = ot::String::toLower(name.substr(name.find_last_of('.') + 1));

	// Create a new EntityBinaryData on the heap (will be stored in DB)
	EntityBinaryData* dataEntity = new EntityBinaryData(
		this->getModelState()->createEntityUID(),
		this,  // parent is this EntityFileText
		nullptr,  // observer will be set later
		this->getModelState()
	);

	// Set the binary data from the library element
	dataEntity->setData(_libraryElement.getData().data(), _libraryElement.getData().size());

	// Store the data entity to the database FIRST
	dataEntity->storeToDataBase();

	// Now update this entity to reference the stored data entity
	this->setDataEntity(dataEntity->getEntityID(), dataEntity->getEntityStorageVersion());

	// Add the data entity to model entities list
	_modelState.addDataEntity(*this, *dataEntity);

	ot::EncodingGuesser guesser;
	this->setFileProperties(path, name, type);

	// Set text encoding to UTF-8
	this->setTextEncoding(guesser(_libraryElement.getData().data(), _libraryElement.getData().size()));

	// Add additional infos as properties
	for (const auto& additionalInfos : _libraryElement.getAdditionalInfos()) {
		EntityPropertiesString* additionalInfoProp = EntityPropertiesString::createProperty(
			"Metadata",
			additionalInfos.first,
			additionalInfos.second,
			"Default",
			this->getProperties()
		);
		additionalInfoProp->setReadOnly(true);
	}

	std::string environmentInfo = _libraryElement.getAdditionalInfoValue("Environment");
	if (!environmentInfo.empty()) {
		// Create the LibraryElementRequest configuration
		ot::LibraryElementRequest request;
		request.setRequestingEntityID(_libraryElement.getRequestingEntityID());
		request.setCollectionName("PythonEnvironments");
		request.setCallBackAction(OT_ACTION_CMD_LMS_LibraryElementRequest);
		request.setEntityType(EntityPythonManifest::className());
		request.setNewEntityFolder(ot::FolderNames::PythonManifestFolder);
		request.setPropertyName("Environment");
		request.setCallBackService(_libraryElement.getCallBackService());
		request.setValue(environmentInfo);  // Now passing string directly

		// Build the document
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LMS_LibraryElementRequest, doc.GetAllocator()), doc.GetAllocator());
		// Add the config information to the document
		ot::JsonObject configObj;
		request.addToJsonObject(configObj, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_Config, configObj, doc.GetAllocator());

		// Send the document to the observer (Model Service)
		getObserver()->requestLibraryElement(doc);
	}
}
