#include "EntityBlockDatabaseAccess.h"
#include "OpenTwinCommunication/ActionTypes.h"

EntityBlockDatabaseAccess::EntityBlockDatabaseAccess(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner)
	:EntityBlock(ID,parent,obs,ms,factory,owner)
{
}

void EntityBlockDatabaseAccess::addVisualizationNodes(void)
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;

		treeIcons.visibleIcon = "BlockDataBaseAccess";
		treeIcons.hiddenIcon = "BlockDataBaseAccess";

		OT_rJSON_createDOC(doc);
		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddContainerNode);
		ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());

		treeIcons.addToJsonDoc(&doc);

		getObserver()->sendMessageToViewer(doc);
	}
}

void EntityBlockDatabaseAccess::createProperties(std::list<std::string>& dataProjectNames, const std::string& defaultProjectName)
{
	EntityPropertiesSelection::createProperty("Database Properties", "Projectname", dataProjectNames, defaultProjectName, "default", getProperties());
	EntityPropertiesString::createProperty("Database Access", "Query", "{}", "default", getProperties());
	EntityPropertiesString::createProperty("Database Access", "Projection", "{value: 1, _id: 0}", "default", getProperties());
}

std::string EntityBlockDatabaseAccess::getSelectedProjectName()
{
	auto propertyBase =	getProperties().getProperty("Projectname");
	auto selectedProjectName = dynamic_cast<EntityPropertiesSelection*>(propertyBase); 
	assert(selectedProjectName != nullptr);
	
	return  selectedProjectName->getValue();
}

std::string EntityBlockDatabaseAccess::getQuery()
{
	auto propertyBase = getProperties().getProperty("Query");
	auto dbQuery = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(dbQuery != nullptr);

	return dbQuery->getValue();
}

std::string EntityBlockDatabaseAccess::getProjection()
{
	auto propertyBase = getProperties().getProperty("Projection");
	auto dbProjection = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(dbProjection != nullptr);

	return dbProjection->getValue();
}

void EntityBlockDatabaseAccess::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockDatabaseAccess::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}
