// @otlicense

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/Properties/PropertyHelper.h"
#include "OTModelEntities/ProjectCompare/EntityProjectCompareNewEntity.h"

static EntityFactoryRegistrar<ot::EntityProjectCompareNewEntity> registrar(ot::EntityProjectCompareNewEntity::className());

ot::EntityProjectCompareNewEntity::EntityProjectCompareNewEntity(ot::UID _id, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms) :
	EntityBase(_id, _parent, _obs, _ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Tree/NewFile.png");
	treeItem.setHiddenIcon("Tree/NewFile.png");
	this->setDefaultTreeItem(treeItem);
}

ot::EntityProjectCompareNewEntity::~EntityProjectCompareNewEntity()
{}

void ot::EntityProjectCompareNewEntity::createProperties()
{
	// Base project properties
	EntityPropertiesString::createProperty("General", "Entity Type", "", "", getProperties())->setReadOnly(true);

	getProperties().forceResetUpdateForAllProperties();
}

bool ot::EntityProjectCompareNewEntity::updateFromProperties()
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	bool updatePropertiesGrid = false;





	return updatePropertiesGrid;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Property setter / getter

void ot::EntityProjectCompareNewEntity::setCompareEntityType(const std::string& _entityType)
{
	PropertyHelper::setStringPropertyValue(_entityType, this, "Entity Type", "General");
}

std::string ot::EntityProjectCompareNewEntity::getCompareEntityType() const
{
	return PropertyHelper::getStringPropertyValue(this, "Entity Type", "General");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected base class methods

void ot::EntityProjectCompareNewEntity::addStorageData(bsoncxx::builder::basic::document& storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now we store the particular information about the current object

}

void ot::EntityProjectCompareNewEntity::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	resetModified();
}
