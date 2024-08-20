#include "EntityBlockCircuitElement.h"

EntityBlockCircuitElement::EntityBlockCircuitElement(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner) 
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	createProperties();
}

void EntityBlockCircuitElement::createProperties()
{
	EntityPropertiesDouble::createProperty("Transform-Properties", "Rotation", 0.0, "default", getProperties());
	EntityPropertiesSelection::createProperty("Transform-Properties", "Flip", { "NoFlip" , "FlipVertically" , "FlipHorizontally" }, "NoFlip", "default", getProperties());
	EntityPropertiesString::createProperty("General", "Name", "Element", "default", getProperties());
}

const std::string&  EntityBlockCircuitElement::getName()
{
	auto propertyBase = getProperties().getProperty("Name");
	auto propertyFlip = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyFlip->getValue();
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