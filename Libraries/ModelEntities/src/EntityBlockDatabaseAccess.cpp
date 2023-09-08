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

void EntityBlockDatabaseAccess::createProperties()
{
	EntityPropertiesProjectList* projectList = new EntityPropertiesProjectList("Projectname");
	getProperties().createProperty(projectList, "Database Access");
	
	EntityPropertiesSelection::createProperty("Access Type", "Outcome dimension", { "1D", "2D", "3D", "Custom" }, "1D", "default", getProperties());
}

std::string EntityBlockDatabaseAccess::getSelectedProjectName()
{
	auto propertyBase =	getProperties().getProperty("Projectname");
	auto selectedProjectName = dynamic_cast<EntityPropertiesProjectList*>(propertyBase); 
	assert(selectedProjectName != nullptr);
	
	return  selectedProjectName->getValue();
}

std::string EntityBlockDatabaseAccess::getQueryDimension()
{
	auto propertyBase = getProperties().getProperty("Outcome dimension");
	auto outcome= dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(outcome != nullptr);

	return outcome->getValue();
}

void EntityBlockDatabaseAccess::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockDatabaseAccess::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}
