// @otlicense

#include "EntityBlockCircuitElement.h"
#include "OTCommunication/ActionTypes.h"

EntityBlockCircuitElement::EntityBlockCircuitElement(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner) 
	:EntityBlock(ID, parent, obs, ms, owner)
{
	
}

void EntityBlockCircuitElement::createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID)
{
	EntityPropertiesDouble::createProperty("Transform-Properties", "Rotation", 0.0, "default", getProperties());
	EntityPropertiesSelection::createProperty("Transform-Properties", "Flip", { "NoFlip" , "FlipVertically" , "FlipHorizontally" }, "NoFlip", "default", getProperties());
	EntityPropertiesSelection::createProperty("Model-Properties", "ModelSelection", { "LoadFromLibrary",""}, "", "default", getProperties());
}

bool EntityBlockCircuitElement::updateFromProperties(void) {
	auto rotationProperty = getProperties().getProperty("Rotation");
	auto flipProperty = getProperties().getProperty("Flip");

	if (rotationProperty->needsUpdate() || flipProperty->needsUpdate()) {
		createBlockItem();
	}

	// Check if LoadFromLibrary was selected
	auto basePropertyModel = getProperties().getProperty("ModelSelection");
	auto modelProperty = dynamic_cast<EntityPropertiesSelection*>(basePropertyModel);
	if (modelProperty == nullptr) {
		OT_LOG_E("Model selection property cast failed");
		return false;
	}
	
	if (modelProperty->getValue() == "LoadFromLibrary") {

		// if it was selected use observer to send message to LMS
		getObserver()->requestConfigForModelDialog(this->getEntityID(),this->getCollectionType(), this->getCircuitModelFolder() + this->getFolderName(), this->getFolderName());
	}

	return true;
}

std::string EntityBlockCircuitElement::getCircuitModel() {
	auto propertyBase = getProperties().getProperty("ModelSelection");
	auto propertyCircuitModel = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyCircuitModel->getValue();
	return value;
}

void EntityBlockCircuitElement::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::addStorageData(storage);
}

void EntityBlockCircuitElement::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}