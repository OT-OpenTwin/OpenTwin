#include "EntityGraphicsScene.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/VisualisationTypes.h"

EntityGraphicsScene::EntityGraphicsScene(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityContainer(ID,parent,obs,ms,factory,owner)
{
}

ot::GraphicsNewEditorPackage* EntityGraphicsScene::getGraphicsEditorPackage()
{
	ot::GraphicsNewEditorPackage* editor = new ot::GraphicsNewEditorPackage(getName(), getName());
	return editor;
}

bool EntityGraphicsScene::visualiseGraphicsView()
{
	return true;
}

void EntityGraphicsScene::addVisualizationNodes()
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "ContainerVisible";
	treeIcons.hiddenIcon = "ContainerHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	ot::VisualisationTypes visTypes;
	visTypes.addGraphicsViewVisualisation();
	visTypes.addToJsonObject(doc, doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}

}
