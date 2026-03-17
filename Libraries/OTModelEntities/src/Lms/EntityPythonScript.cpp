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
#include "OTCommunication/ActionTypes.h"
#include "OTCore/FolderNames.h"
#include "OTCore/EncodingGuesser.h"

static EntityFactoryRegistrar<EntityPythonScript> registrar(EntityPythonScript::className());

void EntityPythonScript::setLibraryElement(const ot::LibraryElement& _libraryElement) {
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

	//// Hide default properties
	//EntityPropertiesBase* pathProp = this->getProperties().getProperty("Path", "Selected File");
	//EntityPropertiesBase* filenameProp = this->getProperties().getProperty("Filename", "Selected File");
	//EntityPropertiesBase* fileTypeProp = this->getProperties().getProperty("FileType", "Selected File");
	//EntityPropertiesBase* encodingProp = this->getProperties().getProperty("Text Encoding", "Text Properties");
	//EntityPropertiesBase* highlightProp = this->getProperties().getProperty("Syntax Highlight", "Text Properties");

	//if (pathProp) pathProp->setVisible(false);
	//if (filenameProp) filenameProp->setVisible(false);
	//if (fileTypeProp) fileTypeProp->setVisible(false);
	//if (encodingProp) encodingProp->setVisible(false);
	//if (highlightProp) highlightProp->setVisible(false);
	// Additional handling specific to EntityPythonScript can be added here if needed

	std::string environmentInfo = _libraryElement.getAdditionalInfoValue("Environment");
	if (!environmentInfo.empty()) {
		// If there is environment information, we can store it as additional info in the entity
		ot::LibraryElementSelectionCfg config;
		config.setRequestingEntityID(_libraryElement.getRequestingEntityID());
		config.setCollectionName("PythonEnvironments");
		config.setCallBackAction(OT_ACTION_CMD_LMS_CreateConfig);
		config.setEntityType(EntityFileText::className());
		config.setNewEntityFolder(ot::FolderNames::PythonManifestFolder);
		config.setPropertyName("Environment");

		getObserver()->requestConfigForModelDialog(config);
	}
}
