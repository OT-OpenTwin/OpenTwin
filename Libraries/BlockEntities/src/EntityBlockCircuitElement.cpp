#include "EntityBlockCircuitElement.h"

EntityBlockCircuitElement::EntityBlockCircuitElement(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner) 
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	
}

void EntityBlockCircuitElement::createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID)
{
	EntityPropertiesDouble::createProperty("Transform-Properties", "Rotation", 0.0, "default", getProperties());
	EntityPropertiesSelection::createProperty("Transform-Properties", "Flip", { "NoFlip" , "FlipVertically" , "FlipHorizontally" }, "NoFlip", "default", getProperties());
	EntityPropertiesEntityList::createProperty("Model-Properties", "Model", _circuitModelFolderName, _circuitModelFolderID, "", -1, "default", getProperties());
}

bool EntityBlockCircuitElement::updateFromProperties(void) {
	auto rotationProperty = getProperties().getProperty("Rotation");
	auto flipProperty = getProperties().getProperty("Flip");

	if (rotationProperty->needsUpdate() || flipProperty->needsUpdate()) {
		CreateBlockItem();
		
	}
	return true;
}

std::string EntityBlockCircuitElement::getCircuitModel() {
	auto propertyBase = getProperties().getProperty("Model");
	auto propertyCircuitModel = dynamic_cast<EntityPropertiesEntityList*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyCircuitModel->getValueName();
	return value;
}



void EntityBlockCircuitElement::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockCircuitElement::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}