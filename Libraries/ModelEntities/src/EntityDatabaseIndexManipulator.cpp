// @otlicense

#include "EntityDatabaseIndexManipulator.h"
#include "PropertyHelper.h"

static EntityFactoryRegistrar<EntityDatabaseIndexManipulator> registrar (EntityDatabaseIndexManipulator::className());

void EntityDatabaseIndexManipulator::createProperties(const std::string& _projectName)
{
	EntityPropertiesProjectList* projectList = new EntityPropertiesProjectList("Project name");
	projectList->setValue(_projectName);
	getProperties().createProperty(projectList, "Project");
}

void EntityDatabaseIndexManipulator::removeAllIndexes()
{
	EntityProperties& allProperties =  getProperties();
	for(EntityPropertiesBase* property : allProperties.getListOfAllProperties())
	{
		if (property->getName() != "Project")
		{
			allProperties.deleteProperty(property->getName());
		}
	}
}

void EntityDatabaseIndexManipulator::createIndexes(const std::list<std::string>& _newIndexNames)
{
	for (const std::string& newIndexName : _newIndexNames)
	{
		if (!getProperties().propertyExists(newIndexName))
		{
			EntityPropertiesString::createProperty("Indexes", newIndexName,"Ascending","default", getProperties());
		}
		else
		{
			OT_LOG_W("Index \"" + newIndexName + "\" already exists.");
		}
	}
}


const std::string EntityDatabaseIndexManipulator::getSelectedProjectName()
{
	const std::string projectName = PropertyHelper::getProjectPropertyValue(this, "Project name", "Project");
	return projectName;
}

