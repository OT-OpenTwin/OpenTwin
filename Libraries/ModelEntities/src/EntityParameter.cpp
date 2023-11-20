// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "EntityParameter.h"
#include "DataBase.h"
#include "Types.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityParameter::EntityParameter(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner)
{
	
}

EntityParameter::~EntityParameter()
{
}

bool EntityParameter::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityParameter::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Add the dependency information to the storage

	auto arrayUID      = bsoncxx::builder::basic::array();
	auto arrayProperty = bsoncxx::builder::basic::array();

	for (auto &entityDependency : dependencyMap)
	{
		for (auto &propertyDependency : entityDependency.second)
		{
			arrayUID.append((long long) entityDependency.first);
			arrayProperty.append(propertyDependency.first);
		}
	}

	storage.append(
		bsoncxx::builder::basic::kvp("dependUID",  arrayUID),
		bsoncxx::builder::basic::kvp("dependProp", arrayProperty)
	);
}

void EntityParameter::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now load the dependency information from the storage
	dependencyMap.clear();

	try
	{
		auto arrayUID      = doc_view["dependUID"].get_array().value;
		auto arrayProperty = doc_view["dependProp"].get_array().value;

		size_t nDependencies = std::distance(arrayUID.begin(), arrayUID.end());
		assert(nDependencies == std::distance(arrayProperty.begin(), arrayProperty.end()));

		auto iUID = arrayUID.begin();
		auto iProperty = arrayProperty.begin();

		for (unsigned long index = 0; index < nDependencies; index++)
		{
			addDependency(iUID->get_int64(), iProperty->get_utf8().value.to_string());

			iUID++;
			iProperty++;
		}
	}
	catch (std::exception)
	{
	}

	resetModified();
}

void EntityParameter::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

void EntityParameter::addVisualizationItem(bool isHidden)
{
	TreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "ParameterVisible";
	treeIcons.hiddenIcon = "ParameterHidden";

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddContainerNode);
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());

	treeIcons.addToJsonDoc(&doc);

	getObserver()->sendMessageToViewer(doc);
}

void EntityParameter::createProperties(void)
{
	EntityPropertiesString::createProperty("General", "Value", "0", "", getProperties());
	EntityPropertiesString::createProperty("General", "Description", "", "", getProperties());

	EntityPropertiesDouble *doubleProp = new EntityPropertiesDouble("#Value", 0.0);
	doubleProp->setVisible(false);

	getProperties().createProperty(doubleProp, "");

	getProperties().forceResetUpdateForAllProperties();
}

void EntityParameter::setValue(const std::string &value)
{
	EntityPropertiesString *stringProperty = dynamic_cast<EntityPropertiesString *>(getProperties().getProperty("Value"));

	if (stringProperty != nullptr)
	{
		stringProperty->setValue(value);
	}
	else
	{
		assert(0);
	}
}

std::string EntityParameter::getValue(void)
{
	EntityPropertiesString *stringProperty = dynamic_cast<EntityPropertiesString *>(getProperties().getProperty("Value"));

	if (stringProperty != nullptr)
	{
		return stringProperty->getValue();
	}
	
	assert(0);
	return "";
}

double EntityParameter::getNumericValue(void)
{
	EntityPropertiesDouble *doubleProperty = dynamic_cast<EntityPropertiesDouble *>(getProperties().getProperty("#Value"));

	if (doubleProperty != nullptr)
	{
		return doubleProperty->getValue();
	}

	assert(0);
	return 0.0;
}

bool EntityParameter::updateFromProperties(void)
{
	setModified();
	getProperties().forceResetUpdateForAllProperties();  // We don't need to do anything special here. The necessary updates with be done in the calling function

	return false;
}

void EntityParameter::addDependency(ot::UID entityID, const std::string &propertyName)
{
	if (dependencyMap.count(entityID) > 0)
	{
		if (dependencyMap[entityID].count(propertyName) > 0)
		{
			// This dependency already exists -> nothing to change
			return;
		}
	}

	dependencyMap[entityID][propertyName] = true;
	setModified();
}

void EntityParameter::removeDependency(ot::UID entityID, const std::string &propertyName)
{
	if (dependencyMap.count(entityID) > 0)
	{
		if (dependencyMap[entityID].count(propertyName) > 0)
		{
			dependencyMap[entityID].erase(propertyName);

			if (dependencyMap[entityID].empty())
			{
				dependencyMap.erase(entityID);
			}

			setModified();
		}
	}
}

void EntityParameter::removeDependency(ot::UID entityID)
{
	if (dependencyMap.count(entityID) > 0)
	{
		dependencyMap.erase(entityID);
		setModified();
	}
}

bool EntityParameter::hasDependency(ot::UID entityID)
{
	return (dependencyMap.count(entityID) > 0);
}

void EntityParameter::removeAllDependencies(void)
{
	dependencyMap.clear();
	setModified();
}

