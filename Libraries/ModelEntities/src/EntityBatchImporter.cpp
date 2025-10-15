#include "EntityBatchImporter.h"
#include "PropertyHelper.h"
#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntityBatchImporter> registrar(EntityBatchImporter::className());

EntityBatchImporter::EntityBatchImporter(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner)
	:EntityBase(_ID, _parent, _obs, _ms, _owner)
{
}

void EntityBatchImporter::createProperties(void)
{
	EntityPropertiesString::createProperty("Categorisation", "Name base", "", "default", getProperties());
	EntityPropertiesInteger::createProperty("Run", "Repetitions", 1, "default", getProperties());
	PropertyHelper::getIntegerProperty(this, "Repetitions")->setMin(1);
}

void EntityBatchImporter::addVisualizationNodes(void)
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "BatchProcessing";
	treeIcons.hiddenIcon = "BatchProcessing";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);
}

uint32_t EntityBatchImporter::getNumberOfRuns()
{
	int32_t repetitions =	PropertyHelper::getIntegerPropertyValue(this, "Repetitions", "Run");
	return static_cast<uint32_t>(repetitions);
}

std::string EntityBatchImporter::getNameBase()
{
	const std::string nameBase = PropertyHelper::getStringPropertyValue(this, "Name base", "Categorisation");
	return nameBase;
}
