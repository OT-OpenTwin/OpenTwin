// @otlicense
// File: EntityBlockCircuitElement.cpp
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

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitElement.h"
#include "OTModelEntities/EntityContainer.h"
#include "OTCore/FolderNames.h"

EntityBlockCircuitElement::EntityBlockCircuitElement(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) 
	:EntityBlock(ID, parent, obs, ms)
{
	
}

void EntityBlockCircuitElement::createProperties()
{
	EntityPropertiesDouble::createProperty("Transform-Properties", "Rotation", 0.0, "default", getProperties());
	EntityPropertiesSelection::createProperty("Transform-Properties", "Flip", { "NoFlip" , "FlipVertically" , "FlipHorizontally" }, "NoFlip", "default", getProperties());
	/*EntityPropertiesSelection::createProperty("Model-Properties", "ModelSelection", { "LoadFromLibrary",""}, "", "default", getProperties());*/
	EntityPropertiesExtendedEntityList::createProperty("Model-Properties", "ModelSelection", ot::FolderNames::CircuitModelsFolder + "/" + getFolderName(), ot::invalidUID, "", -1, {"LoadFromLibrary"},{""}, "default", getProperties());
}

bool EntityBlockCircuitElement::updateFromProperties(void) {
	auto rotationProperty = getProperties().getProperty("Rotation");
	auto flipProperty = getProperties().getProperty("Flip");

	if (rotationProperty->needsUpdate() || flipProperty->needsUpdate()) {
		createBlockItem();
	}

	// Check if LoadFromLibrary was selected
	auto basePropertyModel = getProperties().getProperty("ModelSelection");
	auto modelProperty = dynamic_cast<EntityPropertiesExtendedEntityList*>(basePropertyModel);
	if (modelProperty == nullptr) {
		OT_LOG_E("Model selection property cast failed");
		return false;
	}
	
	if (modelProperty->getValueName() == "LoadFromLibrary") {

		// if it was selected use observer to send message to LMS
		getObserver()->requestConfigForModelDialog(this->getEntityID(),this->getCollectionType(), this->getCircuitModelFolder() + "/" + this->getFolderName(), this->getFolderName());
	}

	return true;
}

EntityNamingBehavior EntityBlockCircuitElement::getNamingBehavior() const {
	EntityNamingBehavior namingBehavior;
	namingBehavior.alwaysNumbered = true;
	namingBehavior.delimiter = "";
	return namingBehavior;
}

std::string EntityBlockCircuitElement::getCircuitModel() {
	auto propertyBase = getProperties().getProperty("ModelSelection");
	auto propertyCircuitModel = dynamic_cast<EntityPropertiesExtendedEntityList*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyCircuitModel->getValueName();
	return value;
}

void EntityBlockCircuitElement::setCircuitModelFolder(ot::UID _folderID) {
	auto basePropertyModel = getProperties().getProperty("ModelSelection");
	auto modelProperty = dynamic_cast<EntityPropertiesExtendedEntityList*>(basePropertyModel);
	if (modelProperty == nullptr) {
		OT_LOG_E("Model selection property cast failed");
		return;
	}

	modelProperty->setEntityContainerID(_folderID);
}

const ot::UID EntityBlockCircuitElement::getCircuitModelFolderID() {
	EntityBase* entBase = getEntityFromName(this->getCircuitModelFolder());
	
	if (!entBase) {
		OT_LOG_E("Could not find Circuit Model folder id: " + getCircuitModelFolder());
		return ot::invalidUID;
	}

	EntityContainer* entContainer = dynamic_cast<EntityContainer*>(entBase);
	if (!entContainer) {
		OT_LOG_E("Dynamic cast to EntityContainer failed for folder: " + getCircuitModelFolder());
		return ot::invalidUID;
	}

	return entContainer->getEntityID();
}

void EntityBlockCircuitElement::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::addStorageData(storage);
}

void EntityBlockCircuitElement::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}