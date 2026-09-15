// @otlicense

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/Properties/PropertyHelper.h"
#include "OTModelEntities/ProjectCompare/EntityProjectCompare.h"

static EntityFactoryRegistrar<ot::EntityProjectCompare> registrar(ot::EntityProjectCompare::className());

ot::EntityProjectCompare::EntityProjectCompare(ot::UID _id, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms) :
	EntityBase(_id, _parent, _obs, _ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Tree/Compare");
	treeItem.setHiddenIcon("Tree/Compare");
	treeItem.setIsEditable(true);
	treeItem.setSelectChilds(false);
	this->setDefaultTreeItem(treeItem);
}

ot::EntityProjectCompare::~EntityProjectCompare()
{}

void ot::EntityProjectCompare::createProperties()
{
	// Base project properties
	EntityPropertiesString::createProperty("Base", "Project", "", "", getProperties())->setReadOnly(true);
	EntityPropertiesString::createProperty("Base", "Version", "", "", getProperties())->setReadOnly(true);

	// Compare project properties
	EntityPropertiesString::createProperty("Comparison", "Project", "", "", getProperties())->setReadOnly(true);
	EntityPropertiesString::createProperty("Comparison", "Version", "", "", getProperties())->setReadOnly(true);

	// Compare settings
	EntityPropertiesBoolean::createProperty("Settings", "Compare Properties", false, "", getProperties())->setReadOnly(true);
	EntityPropertiesBoolean::createProperty("Settings", "Compare Content", false, "", getProperties())->setReadOnly(true);
	EntityPropertiesBoolean::createProperty("Settings", "Compare Results", false, "", getProperties())->setReadOnly(true);

	getProperties().forceResetUpdateForAllProperties();
}

bool ot::EntityProjectCompare::updateFromProperties()
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

void ot::EntityProjectCompare::setBaseProject(const std::string& _projectName)
{
	PropertyHelper::setStringPropertyValue(_projectName, this, "Project", "Base");
}

std::string ot::EntityProjectCompare::getBaseProject() const
{
	return PropertyHelper::getStringPropertyValue(this, "Project", "Base");
}

void ot::EntityProjectCompare::setBaseVersion(const std::string& _version)
{
	PropertyHelper::setStringPropertyValue(_version, this, "Version", "Base");
}

std::string ot::EntityProjectCompare::getBaseVersion() const
{
	return PropertyHelper::getStringPropertyValue(this, "Version", "Base");
}

void ot::EntityProjectCompare::setCompareProject(const std::string& _projectName)
{
	PropertyHelper::setStringPropertyValue(_projectName, this, "Project", "Comparison");
}

std::string ot::EntityProjectCompare::getCompareProject() const
{
	return PropertyHelper::getStringPropertyValue(this, "Project", "Comparison");
}

void ot::EntityProjectCompare::setCompareVersion(const std::string& _version)
{
	PropertyHelper::setStringPropertyValue(_version, this, "Version", "Comparison");
}

std::string ot::EntityProjectCompare::getCompareVersion() const
{
	return PropertyHelper::getStringPropertyValue(this, "Version", "Comparison");
}

void ot::EntityProjectCompare::setCompareProperties(bool _compareProperties)
{
	PropertyHelper::setBoolPropertyValue(_compareProperties, this, "Compare Properties", "Settings");
}

bool ot::EntityProjectCompare::getCompareProperties() const
{
	return PropertyHelper::getBoolPropertyValue(this, "Compare Properties", "Settings");
}

void ot::EntityProjectCompare::setCompareContent(bool _compareContent)
{
	PropertyHelper::setBoolPropertyValue(_compareContent, this, "Compare Content", "Settings");
}

bool ot::EntityProjectCompare::getCompareContent() const
{
	return PropertyHelper::getBoolPropertyValue(this, "Compare Content", "Settings");
}

void ot::EntityProjectCompare::setCompareResults(bool _compareResults)
{
	PropertyHelper::setBoolPropertyValue(_compareResults, this, "Compare Results", "Settings");
}

bool ot::EntityProjectCompare::getCompareResults() const
{
	return PropertyHelper::getBoolPropertyValue(this, "Compare Results", "Settings");
}

void ot::EntityProjectCompare::setPropertiesFromConfig(const ot::ProjectCompareConfig& _config)
{
	setCompareProject(_config.getTargetProjectName());
	setCompareVersion(_config.getTargetProjectVersion());
	
	setCompareProperties(_config.getFlags().has(ot::ProjectCompareConfig::ProjectCompareFlag::CompareProperties));
	setCompareContent(_config.getFlags().has(ot::ProjectCompareConfig::ProjectCompareFlag::CompareData));
	setCompareResults(_config.getFlags().has(ot::ProjectCompareConfig::ProjectCompareFlag::CompareResults));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected base class methods

void ot::EntityProjectCompare::addStorageData(bsoncxx::builder::basic::document& storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now we store the particular information about the current object

}

void ot::EntityProjectCompare::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	resetModified();
}
