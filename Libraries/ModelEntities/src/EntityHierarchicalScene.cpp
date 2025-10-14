//! @file EntityHierarchicalScene.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#include "EntityHierarchicalScene.h"
#include "OTGui/VisualisationTypes.h"
#include "OTCommunication/ActionTypes.h"

EntityHierarchicalScene::EntityHierarchicalScene(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner) 
	: EntityContainer(_ID, _parent, _obs, _ms, _owner)
{

}

ot::GraphicsNewEditorPackage* EntityHierarchicalScene::getGraphicsEditorPackage() {
	ot::GraphicsNewEditorPackage* package = new ot::GraphicsNewEditorPackage(this->getName(), this->getName());
	return package;
}

void EntityHierarchicalScene::addVisualizationNodes() {
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "Hierarchical";
	treeIcons.hiddenIcon = "Hierarchical";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	ot::VisualisationTypes visTypes;
	visTypes.addGraphicsViewVisualisation();
	visTypes.addToJsonObject(doc, doc.GetAllocator());
	treeIcons.addToJsonDoc(doc);

	OTAssertNullptr(this->getObserver());
	this->getObserver()->sendMessageToViewer(doc);

	// Visualize children
	for (EntityBase* child : getChildrenList()) {
		child->addVisualizationNodes();
	}
}
